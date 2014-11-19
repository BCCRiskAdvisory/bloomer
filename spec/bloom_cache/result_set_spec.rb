require 'rspec'
require 'bloom_cache'

describe ResultSet do

  it 'should clear results' do
    rs = ResultSet.create(10)
    expect(rs.to_a).to eq []
    rs.concat([1,2,3,4])
    expect(rs.to_a).to eq [1,2,3,4]
    rs.clear
    expect(rs.to_a).to eq []
  end

  it 'should merge results correctly' do
    rs = ResultSet.create(10)

    ResultSet.merge_results([[1,2,3,4]], rs)
    expect(rs.to_a).to eq [1,2,3,4]

    rs.clear
    ResultSet.merge_results([[1,2,3,4], [2,3,4]], rs)
    expect(rs.to_a).to eq [2,3,4]

    rs.clear
    ResultSet.merge_results([[4,6,7,8,9], [4,5,6,8,9], [7,8,9]], rs)
    expect(rs.to_a).to eq [8,9]

    rs.clear
    arrs = []
    (rand(10) + 5).times do
      arrs.push((rand(10) + 5).downto(0).map{ rand(20) }.sort)
    end
    ResultSet.merge_results(arrs, rs)
    expect(rs.to_a).to eq arrs.inject(:&)

  end
end