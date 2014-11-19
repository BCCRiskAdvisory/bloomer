require 'ffi'

module BloomCacheFFI
  extend FFI::Library    
  ffi_lib $LOAD_PATH.map{|p| p + '/bloom_cache/bloom_cache.so'}
  attach_function :create_bloom_cache, [:int, :int, :int], :pointer
  attach_function :print_bloom_cache, [:pointer], :void
  attach_function :add_value_to_bucket, [:pointer, :int, :int, :int], :void
  attach_function :add_element, [:pointer, :pointer, :int, :int], :void
  attach_function :retrieve_elements, [:pointer, :pointer], :pointer
  attach_function :add_value, [:pointer, :pointer, :int, :int, :int], :void
  attach_function :match_elements, [:pointer, :pointer, :int], :pointer
  attach_function :delete_bloom_cache, [:pointer], :void
  attach_function :delete_result_set, [:pointer], :void
  attach_function :sort_buckets, [:pointer], :void
  attach_function :clear_cache, [:pointer], :void
  attach_function :merge_results, [:pointer, :pointer, :int, :pointer], :void
  attach_function :create_result_set, [:int], :pointer
  attach_function :add_results, [:pointer, :pointer, :int], :void
end

class ResultSet < FFI::ManagedStruct
  layout :data, :pointer,
    :count, :int,
    :size, :int,
    :size_step, :int

  [:map, :each].each do |meth|
    define_method meth do |*args, &block|
      to_a.send(meth, *args, &block)
    end
  end

  def push(*vals)
    concat(vals.flatten)
  end

  def concat(other)
    raise ArgumentError('Target must convert to an array') if !other.respond_to?(:to_a)
    arr = other.to_a
    p = FFI::MemoryPointer.new(:int32, arr.length)
    p.write_array_of_int32(arr)
    BloomCacheFFI.add_results(pointer, p, arr.length)
  end

  def length
    self[:count]
  end

  def count
    self[:count]
  end

  def to_a
    self[:data].get_array_of_int(0, self[:count])
  end

  def self.create(initial_size)
    ResultSet.new(BloomCacheFFI.create_result_set(initial_size))
  end

  def self.release(ptr)
    BloomCacheFFI.delete_result_set(ptr)
  end

  def clear
    self[:count] = 0
  end

  def inspect
    to_a.inspect
  end

  def self.merge_results(results, destination)
    raise ArgumentError('Destination must be a ResultSet') if !destination.is_a? ResultSet      
    res_count_array = FFI::MemoryPointer.new(:int32, results.count)
    res_count_array.write_array_of_int32(results.map(&:length))
    res_pointers = []
    results.each do |res|
      p = FFI::MemoryPointer.new(:int32, res.count)
      p.write_array_of_int32(res)
      res_pointers.push(p)
    end
    res_pointer_buf = FFI::MemoryPointer.new(:pointer, results.count)
    res_pointer_buf.write_array_of_pointer(res_pointers)
    BloomCacheFFI.merge_results(res_pointer_buf, res_count_array, results.length, destination)
  end

end

class BloomCache < FFI::ManagedStruct

  layout :bitwidth, :int,
    :intwidth, :int,
    :buckets, :pointer,
    :bucket_lengths, :pointer,
    :bucket_counts, :pointer,
    :bucket_length_step, :int,
    :max_bucket_count, :int,
    :hash_count, :int

  def buckets
    self[:buckets].get_array_of_pointer(0, self[:bitwidth]).map.with_index do |p, i|
      p.get_array_of_int(0, self[:bucket_counts].get_array_of_int(0, self[:bitwidth])[i])
    end
  end

  def clear
    BloomCacheFFI.clear_cache(pointer)
  end

  def sort_buckets
    BloomCacheFFI.sort_buckets(pointer)
  end

  def self.create(bitwidth, bucket_length_step, hash_count)
    BloomCache.new(BloomCacheFFI.create_bloom_cache(bitwidth, bucket_length_step, hash_count))
  end

  def print
    BloomCacheFFI.print_bloom_cache(pointer)
  end

  def add_value_to_bucket(bucket, value, sort = false)
    val = sort ? 1 : 0
    BloomCacheFFI.add_value_to_bucket(pointer, bucket, value, val)
  end

  def add_value(string, id, sort = false)
    buf = FFI::MemoryPointer.new(:uint8, string.bytes.length)
    buf.put_array_of_uint8(0, string.bytes)
    BloomCacheFFI.add_value(pointer, buf, string.bytes.length, id, sort ? 1 : 0)
  end

  def add_element(bitfield, value)        
    BloomCacheFFI.add_element(pointer, bitfield_to_buffer(bitfield), value)
  end

  def retrieve_elements(bitfield) 
    ResultSet.new(BloomCacheFFI.retrieve_elements(pointer, bitfield_to_buffer(bitfield)))
  end

  def match(string)
    buf = FFI::MemoryPointer.new(:uint8, string.bytes.length)
    buf.put_array_of_uint8(0, string.bytes)
    ResultSet.new(BloomCacheFFI.match_elements(pointer, buf, string.bytes.length))
  end

  def bitfield_to_buffer(bitfield)
    int_width = [(self[:bitwidth].to_i / 32), 1].max
    puts "int_width: #{int_width}"   
    buf = FFI::MemoryPointer.new(:uint32, int_width)
    arr = []
    int_width.times do |i|
      mask = (1 << 32) - 1
      mask <<= (i * 32)
      arr.push((bitfield & mask) >> (i * 32))
    end
    buf.put_array_of_uint32(0, arr)    
    buf
  end

  def self.release(ptr)
    BloomCacheFFI.delete_bloom_cache(ptr)
  end

end