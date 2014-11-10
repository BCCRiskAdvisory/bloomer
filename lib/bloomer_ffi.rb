require 'ffi'

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
    :hash_key_one, :int,
    :hash_key_two, :int,
    :hash_key_three, :int,
    :hash_key_four, :int

  def buckets
    self[:buckets].get_array_of_pointer(0, self[:bitwidth]).map.with_index do |p, i|
      p.get_array_of_int(0, self[:bucket_counts].get_array_of_int(0, self[:bitwidth])[i])
    end
  end

  def self.create(bitwidth, bucket_length_step)
    Bloomer.new(ClassMethods.create_bloomer(bitwidth, bucket_length_step))
  end

  def print
    ClassMethods.print_bloomer(pointer)
  end

  def add_value_to_bucket(bucket, value, sort = false)
    val = sort ? 1 : 0
    ClassMethods.add_value_to_bucket(pointer, bucket, value, val)
  end

  def add_element(bitfield, value)        
    ClassMethods.add_element(pointer, bitfield_to_buffer(bitfield), value)
  end

  def retrieve_elements(bitfield) 
    ResultSet.new(ClassMethods.retrieve_elements(pointer, bitfield_to_buffer(bitfield)))
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
    ffi_lib 'ext/bloomer/bloomer.so'
    attach_function :create_bloomer, [:int, :int], :pointer
    attach_function :print_bloomer, [:pointer], :void
    attach_function :add_value_to_bucket, [:pointer, :int, :int, :int], :void
    attach_function :add_element, [:pointer, :pointer, :int], :void
    attach_function :retrieve_elements, [:pointer, :pointer], :pointer
  end  
end

class String
  def pad_to(bits)
    ('0' * (bits - self.length)) + self
  end
end