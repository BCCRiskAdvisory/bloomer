require 'rspec'
require 'bloomer'

describe Bloomer do
  it 'should store and retrieve records correctly' do
    b = Bloomer.create(256, 32, 4)
    b.add_value('blah', 1)
    b.add_value('bla', 4)
    b.add_value('lba', 6)
    expect(b.match('bla').to_a).to eq [1, 4]
  end

  it 'should store an element in at least one bucket' do
    b = Bloomer.create(64, 8, 4)
    b.add_value('blah', 69)
    expect(b.buckets.count{|bucket| bucket.include? 69}).to gt
  end
end