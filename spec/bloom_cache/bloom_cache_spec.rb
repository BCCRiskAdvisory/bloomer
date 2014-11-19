require 'rspec'
require 'bloom_cache'

describe BloomCache do
  it 'should store and retrieve records correctly' do
    b = BloomCache.create(256, 32, 4)
    b.add_value('blah', 6)
    b.add_value('bla', 4)
    b.add_value('lba', 2)
    b.sort_buckets
    expect(b.match('bla').to_a).to eq [4, 6]
  end

  it 'should store an element in at least one bucket' do
    b = BloomCache.create(64, 8, 4)
    b.add_value('blah', 69)
    expect(b.buckets.count{|bucket| bucket.include? 69}).to be > 0
  end

  it 'should work correctly with different bitwidths' do
    b = BloomCache.create(35, 4, 2)
    b.add_value('sdf', 2)
    b.add_value('wer', 1)
    b.sort_buckets
    expect(b.match('sdf').to_a).to eq [2]
  end

  it 'should clear cache correctly' do
    b = BloomCache.create(32, 4, 2)
    rand_str = ->() do
      (rand(10) + 5).times.map{ ('a'..'z').to_a[rand(26)] }.join
    end
    100.times { b.add_value(rand_str.call, rand(1000)) }
    expect(b.buckets.map{ |bucket| bucket.inject(0, :+) }.inject(0, :+)).to be > 0
    b.clear
    expect(b.buckets.map{ |bucket| bucket.inject(0, :+) }.inject(0, :+)).to eq 0
  end

  it 'should deal with short strings sensibly' do #i.e. without segfaults!
    b = BloomCache.create(32, 4, 2)
    b.add_value('a', 3)
    expect(b.match('a').to_a).to eq [3]

    b.add_value('aa', 4)
    expect(b.match('a').to_a).to eq [3, 4]

    b.add_value('a thing', 5)
    expect(b.match('a').to_a).to eq [3, 4]

  end

end