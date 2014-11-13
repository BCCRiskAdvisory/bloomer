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
end