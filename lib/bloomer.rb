require 'ffi'
require 'bloomer/bloomer'

class ResultSet < FFI::Struct
  layout :data, :pointer,
    :count, :int,
    :size, :int,
    :size_step, :int

    def to_a
      self[:data].get_array_of_int(0, self[:count])
    end

end

class Bloomer < FFI::Struct

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

  def self.create(bitwidth, bucket_length_step, hash_count)
    Bloomer.new(ClassMethods.create_bloomer(bitwidth, bucket_length_step, hash_count))
  end

  def print
    ClassMethods.print_bloomer(pointer)
  end

  def add_value_to_bucket(bucket, value, sort = false)
    val = sort ? 1 : 0
    ClassMethods.add_value_to_bucket(pointer, bucket, value, val)
  end

  def add_value(string, id)
    buf = FFI::MemoryPointer.new(:uint8, string.bytes.length)
    buf.put_array_of_uint8(0, string.bytes)
    ClassMethods.add_value(pointer, buf, string.bytes.length, id)
  end

  def add_element(bitfield, value)        
    ClassMethods.add_element(pointer, bitfield_to_buffer(bitfield), value)
  end

  def retrieve_elements(bitfield) 
    ResultSet.new(ClassMethods.retrieve_elements(pointer, bitfield_to_buffer(bitfield)))
  end

  def match(string)
    buf = FFI::MemoryPointer.new(:uint8, string.bytes.length)
    buf.put_array_of_uint8(0, string.bytes)
    ResultSet.new(ClassMethods.match_elements(pointer, buf, string.bytes.length))
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

  module ClassMethods
    extend FFI::Library
    path = File.dirname(__FILE__).to_s
    puts path
    puts File.expand_path(path, '../../../../../')
    ffi_lib [FFI::CURRENT_PROCESS, 'bloomer']
    attach_function :create_bloomer, [:int, :int, :int], :pointer
    attach_function :print_bloomer, [:pointer], :void
    attach_function :add_value_to_bucket, [:pointer, :int, :int, :int], :void
    attach_function :add_element, [:pointer, :pointer, :int], :void
    attach_function :retrieve_elements, [:pointer, :pointer], :pointer
    attach_function :add_value, [:pointer, :pointer, :int, :int], :void
    attach_function :match_elements, [:pointer, :pointer, :int], :pointer
  end  
end