#include "pch.h"
#include "SimplexMethod.h"
#include "Mathematics.h"

using namespace Util::ORtool;

void Util::ORtool::DenseRow::multiply( double val )
{
	if( Util::EQ( val, 1 ) ) [[unlikely]]
		return;
	for( auto& e : *this )
		e *= val;
}

void Util::ORtool::DenseRow::add( const double coef, const DenseRow& other )
{
	this->resize( std::max( size(), other.size() ), 0 );
	auto x = begin();
	auto y = other.begin();
	for( ; x != end() && y != other.end(); ++x, ++y )
	{
		*x += *y * coef;
	}
}

void Util::ORtool::DenseRow::add( const double coef, const SparseRow& other )
{
	if( other.empty() ) [[unlikely ]]
		return;
	this->resize( std::max( (int)size(), other.back().first + 1 ), 0 );
	for( auto [idx, val] : other )
		this->operator[]( idx ) += coef * val;
}

double Util::ORtool::DenseRow::dot( const DenseRow& other ) const
{
	Util::Math::KahanSummation ks;
	auto x = begin();
	auto y = other.begin();
	for( ; x != end() && y != other.end(); ++x, ++y )
	{
		ks.add( *x * *y );
	}
	return ks.sum;
}

double Util::ORtool::DenseRow::dot( const SparseRow& other ) const
{
	return other.dot( *this );
}

int Util::ORtool::DenseRow::getLength() const
{
	return static_cast<int>( size() );
}

SparseRow Util::ORtool::DenseRow::toSparseRow() const
{
	SparseRow sr;
	sr.reserve( this->size() );
	int k = 0;
	for( auto& e : *this )
	{
		if( !Util::IsZero( e ) )
			sr.emplace_back( k, e );
		++k;
	}
	sr.shrink_to_fit();
	assert( sr.check() );
	return sr;
}

std::string Util::ORtool::DenseRow::toString( const int precision )const
{
	std::ostringstream ss;
	ss.precision( precision );
	for( double val : *this )
		ss << val << ' ';
	return ss.str();
}

//A+=B*ceof


//remove 0 cell

void Util::ORtool::SparseRow::reduce()
{
	std::erase_if( *this, [] ( auto& e )->bool
	{
		return Util::IsZero( e.second );
	} );
}

void Util::ORtool::SparseRow::multiply( double val )
{
	if( Util::EQ( val, 1 ) ) [[unlikely]]
		return;
	for( auto i = raw_type::begin(); i != raw_type::end(); ++i )
		i->second *= val;
}

void Util::ORtool::SparseRow::add( const double coef, const SparseRow& other )
{
	assert( check() );
	assert( other.check() );

	SparseRow sr;
	sr.reserve( size() + other.size() );
	auto x = raw_type::cbegin();
	auto y = other.raw_type::cbegin();
	//merge sort
	while( x != raw_type::cend() && y != other.raw_type::cend() )
	{
		if( x->first == y->first )
		{
			sr.emplace_back( x->first, x->second + y->second * coef );
			++x;
			++y;
		}
		else if( x->first < y->first )
		{
			sr.emplace_back( *x );
			++x;
		}
		else
		{
			sr.emplace_back( y->first, y->second * coef );
			++y;
		}
	}
	if( x != raw_type::cend() )
	{
		sr.insert( sr.end(), x, raw_type::cend() );
	}
	if( y != other.raw_type::cend() )
	{
		for( ; y != other.raw_type::cend(); ++y )
			sr.emplace_back( y->first, y->second * coef );
	}
	sr.swap( *this );
	assert( check() );
}

void Util::ORtool::SparseRow::add( const double coef, const DenseRow& other )
{
	auto tmp = other;
	tmp.add( coef, *this );
	*this = tmp.toSparseRow();
}

double Util::ORtool::SparseRow::dot( const SparseRow& other ) const
{
	assert( check() );
	assert( other.check() );
	Util::Math::KahanSummation ks;
	auto x = raw_type::cbegin();
	auto y = other.raw_type::cbegin();
	//merge sort
	while( x != raw_type::cend() && y != other.raw_type::cend() )
	{
		if( x->first == y->first )
		{
			ks.add( x->second * y->second );
			++x;
			++y;
		}
		else if( x->first < y->first )
			++x;
		else
			++y;
	}
	return ks.sum;
}

double Util::ORtool::SparseRow::dot( const DenseRow& other ) const
{
	assert( check() );
	assert( other.check() );
	Util::Math::KahanSummation ks;
	for( auto [idx, val] : *this )
		ks.add( other[idx] * val );
	return ks.sum;
}

double Util::ORtool::SparseRow::operator[]( const int target )const
{
	if( size() < SEARCH_TRADE_OFF )
	{
		for( auto [idx, val] : *this )
		{
			if( idx == target )
				return val;
		}
		return 0;
	}
	else
	{
		auto tmp = std::lower_bound( raw_type::begin(), raw_type::end(), std::make_pair( target, 0 ), [] ( auto& a, auto& b )->bool
		{
			return a.first < b.first;
		} );
		return ( tmp == raw_type::cend() || tmp->first != target ) ? 0.0 : tmp->second;
	}
}

double& Util::ORtool::SparseRow::operator[]( const int target )
{
	if( size() < SEARCH_TRADE_OFF )
	{
		auto i = raw_type::begin();
		for( ; i != raw_type::end(); ++i )
		{
			if( i->first == target )
				return i->second;
			else if( i->first > target )
				break;
		}
		return this->insert( i, { target,0 } )->second;
	}
	else
	{
		auto tmp = std::lower_bound( raw_type::begin(), raw_type::end(), std::make_pair( target, 0 ), [] ( auto& a, auto& b )->bool
		{
			return a.first < b.first;
		} );
		return ( tmp == raw_type::cend() || tmp->first != target ) ? this->insert( tmp, { target,0 } )->second : tmp->second;
	}
}

//by index

void Util::ORtool::SparseRow::sort()
{
	std::sort( raw_type::begin(), raw_type::end(), [this] ( auto& a, auto& b )->bool
	{
		return a.first < b.first;
	} );
}


//sorted index, same size

bool Util::ORtool::SparseRow::check() const
{
	if( empty() )
		return true;
	if( !std::is_sorted( raw_type::begin(), raw_type::end(), [this] ( auto& a, auto& b )->bool
	{
		return a.first < b.first;
	} ) )
		return false;
	//check dup index
	auto x = raw_type::cbegin();
	auto y = x + 1;
	for( ; y != raw_type::cend(); ++x, ++y )
		if( x->first == y->first )
			return false;
	return true;
}

int Util::ORtool::SparseRow::getLength() const
{
	return this->empty() ? 0 : ( this->back().first + 1 );
}

DenseRow Util::ORtool::SparseRow::toDenseRow( int n ) const
{
	assert( check() );
	if( raw_type::empty() ) [[unlikely]]
		return DenseRow();

	DenseRow dr;
	n = std::max( raw_type::back().first + 1, n );//max index or given size as length of new vector
	dr.resize( n, 0 );
	for( auto [idx, val] : *this )
	{
		dr[idx] = val;
	}
	return dr;
}

std::string Util::ORtool::SparseRow::toString( const int precision )const
{
	std::ostringstream ss;
	ss.precision( precision );
	for( auto [idx, val] : *this )
		ss << '(' << idx << ',' << val << ')' << ' ';
	return ss.str();
}
