#pragma once
#include <vector>
#include <list>
#include <tuple>
#include <numeric>
#include <random>
#include <assert.h>

#include "Util.h"
#include "Timer.h"

namespace Util::ORtool
{
struct DenseRow;
struct SparseRow;

struct DenseRow :public std::vector<double>
{
	using std::vector<double>::vector;

	constexpr bool check()const noexcept	{		return true;	}//dummy
	constexpr void reduce()const noexcept{}//dummy
	void multiply( double val );
	void add( const double coef, const DenseRow& other );
	void add( const double coef, const SparseRow& other );
	double dot( const DenseRow& other )const;
	double dot( const SparseRow& other )const;
	int getLength()const;
	SparseRow toSparseRow()const;
	DenseRow toDenseRow()const	{		return *this;	}
	std::string toString( const int precision = 2 )const;
};
struct SparseRow :protected std::vector<std::pair<int, double>>
{
	static constexpr int SEARCH_TRADE_OFF = 16;//linear search or binary search

	using value_type = std::pair<int, double>;
	using raw_type = std::vector<value_type>;

	using raw_type::size;
	using raw_type::clear;
	using raw_type::reserve;
	using raw_type::resize;
	using raw_type::empty;
	
	friend struct DenseRow;
	friend class SimplexMethod;

	constexpr raw_type::const_iterator begin()const noexcept		{		return raw_type::cbegin();	}
	constexpr raw_type::const_iterator end()const noexcept			{		return raw_type::cend();	}
	constexpr raw_type::value_type get( int idx )const noexcept		{		return raw_type::operator[]( idx );	}
	constexpr void set( int idx, raw_type::value_type val )noexcept	{		raw_type::operator[]( idx ) = val;	}

	SparseRow() :raw_type()
	{}
	template <typename T>
	SparseRow( T st, T ed ) :raw_type()
	{
		raw_type::reserve( std::distance( st, ed ) );
		for( int idx = 0; st != ed; ++st )
		{
			if( !Util::IsZero( *st ) )
				this->emplace_back( idx, *st );
			++idx;
		}
	}

	//remove 0 cell
	void reduce();
	void multiply( double val );
	//A+=B*ceof
	void add( const double coef, const SparseRow& other );
	void add( const double coef, const DenseRow& other );
	double dot( const SparseRow& other )const;
	double dot( const DenseRow& other )const;
	//O(logN) if N>=SEARCH_TRADE_OFF
	double operator[]( const int target )const;
	//O(N) if create, otherwise O(logN)
	double& operator[]( const int target );
	//by index
	void sort();
	//sorted index, same size
	bool check()const;
	int getLength()const;
	DenseRow toDenseRow( int n = 0 )const;
	SparseRow toSparseRow()const	{		return *this;	}
	
	template <typename Condi>
	void erase_if( Condi fn )
	{
		assert( check() );
		std::erase_if( *this, fn );
		assert( check() );
	}
	std::string toString( const int precision = 2 )const;
};
template <typename T>
concept row_type = std::same_as<T, DenseRow> || std::same_as<T, SparseRow>;

//=,>=,<=
enum struct tRelation
{
	kEQ = 0,
	kLE = -1,
	kGE = 1
};

template <row_type T>
struct Equation :public std::tuple<T, tRelation, double>
{
	using std::tuple<T, tRelation, double>::tuple;

	const T& get_vector()const noexcept					{		return std::get<0>( *this );	}
	T& get_vector()noexcept								{		return std::get<0>( *this );	}
	constexpr tRelation get_relation()const noexcept	{		return std::get<1>( *this );	}
	constexpr tRelation& get_relation()noexcept			{		return std::get<1>( *this );	}
	constexpr double get_rhs()const noexcept			{		return std::get<2>( *this );	}
	constexpr double& get_rhs()noexcept					{		return std::get<2>( *this );	}
	//always create, slow
	//ex. x+y+z=1, take x=2 -> y+z=-1
	void substitute( int idx, double val )
	{
		double& tmp = get_vector()[idx];
		get_rhs() -= tmp * val;
		tmp = 0;
	}
	void multiply( double val )
	{
		assert( !Util::IsZero( val ) );
		get_vector().multiply( val );
		get_rhs() *= val;
		if( Util::LT( val, 0 ) )
			reverse_relation();
	}
	template <row_type Tother>
	void add( const double coef, const Equation<Tother>& other )
	{
		get_vector().add( coef, other.get_vector() );
		get_rhs() += coef * other.get_rhs();
	}
	constexpr void reverse_relation()noexcept
	{
		static_assert( static_cast<int>( tRelation::kEQ ) == 0 && static_cast<int>( tRelation::kLE ) + static_cast<int>( tRelation::kGE ) == 0 );
		get_relation() = static_cast<tRelation>( static_cast<int>( get_relation() ) * -1 );
	}
	bool isSatisfied( const T& vec )const
	{
		const double ret = get_vector().dot( vec );
		switch( get_relation() )
		{
		case tRelation::kEQ:
			return Util::EQ( ret, get_rhs() );
		case tRelation::kLE:
			return Util::LE( ret, get_rhs() );
		case tRelation::kGE:
			return Util::GE( ret, get_rhs() );
		default:
			assert( 0 );
			break;
		}
		return false;
	}
	Equation<SparseRow> toSparseEquation()const
	{
		if constexpr( std::same_as<T, DenseRow> )
			return Equation<SparseRow>( get_vector().toSparseRow(), get_relation(), get_rhs() );
		if constexpr( std::same_as<T, SparseRow> )
			return *this;
	}
	Equation<DenseRow> toDenseEquation()const
	{
		if constexpr( std::same_as<T, DenseRow> )
			return *this;
		if constexpr( std::same_as<T, SparseRow> )
			return Equation<DenseRow>( get_vector().toDenseRow(), get_relation(), get_rhs() );
	}
	//<all zero,valid>, if all zero, valid means always true of false
	std::pair<bool, bool> check()const
	{
		bool all_zero = true;
		if constexpr( std::same_as<T, DenseRow> )
		{
			for( double val : get_vector() )
				if( !Util::IsZero( val ) )
				{
					all_zero = false;
					break;
				}
		}
		if constexpr( std::same_as<T, SparseRow> )
		{
			for( auto [idx, val] : get_vector() )
				if( !Util::IsZero( val ) )
				{
					all_zero = false;
					break;
				}
		}

		bool valid = true;
		if( all_zero )
		{
			switch( get_relation() )
			{
			case tRelation::kEQ:
				valid = Util::IsZero( get_rhs() );
				break;
			case tRelation::kGE:
				valid = Util::GE( 0, get_rhs() );
				break;
			case tRelation::kLE:
				valid = Util::LE( 0, get_rhs() );
				break;
			default:
				break;
			}
		}
		return { all_zero,valid };
	}
	std::string toString( const int precision = 2 )const
	{
		std::ostringstream ss;
		ss << get_vector().toString( precision );
		switch( get_relation() )
		{
		case tRelation::kEQ:
			ss << "==";
			break;
		case tRelation::kLE:
			ss << "<=";
			break;
		case tRelation::kGE:
			ss << ">=";
			break;
		default:
			break;
		}
		ss.precision( precision );
		ss << ' ' << get_rhs();
		return ss.str();
	}
};

static constexpr double INF = std::numeric_limits<double>::infinity();
template <row_type T>
struct NonStandardFormLinearProgram :public std::list<Equation<T>>
{
	bool isMaximization = true;
	T objectivefunc;
	std::vector<double> lb;//domain lowerbound, use INF if it is R

	void Clear()
	{
		this->clear();
		objectivefunc.clear();
		lb.clear();
	}
	//O(M)~O(MlogN)
	void Substitute( int idx, double val )
	{
		for( auto& e : *this )
		{
			if constexpr( std::same_as<T, SparseRow> )
			{
				if( Util::IsZero( e.get_vector()[idx] ) )//create is slow
					continue;
			}
			double& tmp = e.get_vector()[idx];
			e.get_rhs() -= tmp * val;
			tmp = 0;
		}
	}
	double CalcOFV( const T& result )const
	{
		return objectivefunc.dot( result );
	}
	template <typename Iter>
	double CalcOFV( Iter st, Iter ed )const
	{
		return CalcOFV( T( st, ed ) );
	}

	bool CheckLowerBound( const T& result )const
	{
		const int n = (int)lb.size();
		if constexpr( std::same_as<T, DenseRow> )
		{
			const int m = std::min( n, (int)result.size() );
			for( int i = 0; i < m; ++i )
				if( Util::LT( result[i], lb[i] ) )
					return false;
		}
		if constexpr( std::same_as<T, SparseRow> )
			for( auto [idx, val] : result )
				if( idx >= 0 && idx < n && Util::LT( val, lb[idx] ) )
					return false;
		return true;
	}
	template <typename Iter>
	bool CheckLowerBound( Iter st, Iter ed )const
	{
		return CheckLowerBound( T( st, ed ) );
	}

	bool CheckEquation( const T& result )const
	{
		for( auto& eq : *this )
			if( !eq.isSatisfied( result ) )
			{
				return false;
			}
		return true;
	}
	template <typename Iter>
	bool CheckEquation( Iter st, Iter ed )const
	{
		return CheckEquation( T( st, ed ) );
	}
	std::string toString( const int precision = 2 )const
	{
		std::ostringstream ss;
		ss.precision( precision );
		ss << ( isMaximization ? "Max " : "Min " );
		ss << objectivefunc.toString( precision ) << '\n';

		ss << "Domain ";
		for( double val : lb )
			if( val == -INF )
				ss << "R ";
			else
				ss << val << ' ';
		ss << '\n';
		for( auto& eq : *this )
			ss << eq.toString( precision ) << '\n';
		return ss.str();
	}
};

//https://www.math.wsu.edu/faculty/dzhang/201/Guideline%20to%20Simplex%20Method.pdf
class SimplexMethod
{
public:
	enum struct tError
	{
		kEmptyDomain,
		kInputNotMatchWithLastResult,
		
		kOptimum,
		kInfeasible,
		kUnbound,
	};

private:
	static constexpr double PerturbationEPS = 1e-14;//consider accumulation of errors, this can't be closed to Util::eps, but not sure if this is suitable
	mutable std::mt19937 rng;//for perturbation, http://theory.stanford.edu/~megiddo/pdf/degen.pdf says almost do not need to worry about PerturbationEPS too small
	double timelimit = std::numeric_limits<double>::max();
	int maxiteration = INT_MAX;
	bool use_perturbation = true;
	bool use_revised_simplex_method = false;

	int phase1_iteration = 0;
	int phase2_iteration = 0;
	mutable int iteration = 0;
	Timer time;

	//mid result and for LP with new constraints
	int n_slack = 0;//for XXX <= a and for XXX >= a
	int n_artificial_var = 0;//for XXX >= a and XXX == a
	int n_additional_constraint = 0;
	double bias = 0;
	std::vector<int> last_idxmap;
	std::vector<double> raw_result;
	std::vector<double> shift;
	struct RecoverInfo
	{
		int var_idx = -1;
		tRelation relation;
		Equation<SparseRow> eq_substitution;
		Equation<SparseRow> eq_recover;
	};
	std::list<RecoverInfo> recoverlist;

public:
	SimplexMethod() :rng( 0 )
	{}
	void SetSeed( unsigned int seed )			{		rng.seed( seed );			}

	bool GetPerturbation()const noexcept		{		return use_perturbation;	}
	void SetPerturbation( bool val )noexcept	{		use_perturbation = val;		}
	
	void SetRevisedSimplexMethod( bool val )noexcept	{		use_revised_simplex_method = val;		}

	int GetPhase1Iteration()const noexcept			{		return phase1_iteration;			}
	int GetPhase2Iteration()const noexcept			{		return phase2_iteration;			}
	int GetTotalIteration()const noexcept			{		return phase1_iteration + phase2_iteration;			}

	void SetMaxIteration( int val )noexcept		{		maxiteration = val;			}

	double GetTimeAsSecond()const				{		return time.GetSeconds();	}
	void SetTimelimit( double val )noexcept		{		timelimit = val;			}
	
	bool isTerminated()const
	{
		return timelimit != std::numeric_limits<double>::max() && ( iteration >= maxiteration || time.GetTime() >= timelimit );
	}
	void Clear()
	{
		n_slack = 0;
		n_artificial_var = 0;
		n_additional_constraint = 0;
		bias = 0;
		last_idxmap.clear();
		raw_result.clear();
		shift.clear();
		recoverlist.clear();
	}
	//<status,ofv>, aim for equivalent result (even multi-solution) with same Dense/Sparse structure
	template <row_type T>
	std::pair<tError, double> SolveLP( NonStandardFormLinearProgram<T>& input, std::vector<double>& result )
	{
		//init
		phase1_iteration = 0;
		phase2_iteration = 0;
		time.SetTime();

		recoverlist.clear();
		bias = 0;

		const int n = (int)input.lb.size();
		shift.clear();
		shift.resize( n, 0 );//x>=a -> y=x-a and y>=0, save 'a' for recovering x

		//check domain
		for( int i = 0; i < n; i++ )
			if( input.lb[i] == INF )
				return std::make_pair( tError::kEmptyDomain, 0 );

		//resize dense row in case size is different
		if constexpr( std::same_as<T, DenseRow> )
		{
			input.objectivefunc.resize( n, 0 );
			for( auto& eq : input )
				eq.get_vector().resize( n, 0 );
		}

		//min->max
		if( !input.isMaximization )
			input.objectivefunc.multiply( -1 );
		
		//convert Non-INF domain to x>=0
		bool need_shift = false;
		for( int i = 0; i < n; i++ )
			if( input.lb[i] != -INF && !Util::IsZero( input.lb[i] ) )
			{
				//since x>=a, let X=x-a, replace x with X+a, reuse current idx, rhs=rhs-x_coef*a
				shift[i] = input.lb[i];
				need_shift = true;
			}
		if( need_shift )
		{
			T shift_row( shift.begin(), shift.end() );
			for( auto& eq : input )
				eq.get_rhs() += RemoveLowerBound( eq.get_vector(), shift_row );
			bias -= RemoveLowerBound( input.objectivefunc, shift_row );
		}

		//convert R domain to x>=0 by substitution (convert equation to '==' first)
		for( int i = 0; i < n; i++ )
			if( input.lb[i] == -INF )
			{
				enum struct tPrio
				{
					kEQ = 1,
					kLEorGE = 2,
					kNull = 99
				};
				//find equation with non-zero value at i
				//priority: 1- '==', 2- same relation and less non-zero coef  ('==' not need to add new var)
				auto select = input.end();
				tPrio prio = tPrio::kNull;
				int cnt = INT_MAX;
				for( auto eq = input.begin(); eq != input.end(); ++eq )
				{
					if( Util::IsZero( eq->get_vector()[i] ) )
						continue;
					const tPrio tmp_prio = ( eq->get_relation() == tRelation::kEQ ) ? tPrio::kEQ : tPrio::kLEorGE;
					int nonzero = 0;
					//more zero cell only improve sparse row
					if constexpr( std::same_as<T, SparseRow> )
					{
						for( auto [idx, val] : eq->get_vector() )
							nonzero += !Util::IsZero( val );
					}
					//make sure equivalent result 
					if constexpr( std::same_as<T, DenseRow> )
					{
						for( double val : eq->get_vector() )
							nonzero += !Util::IsZero( val );
					}
					if( static_cast<int>( tmp_prio ) < static_cast<int>( prio ) || ( tmp_prio == prio && nonzero < cnt ) )
					{
						select = eq;
						prio = tmp_prio;
						cnt = nonzero;
					}
				}
				if( select == input.end() )
					continue;

				//substitute i with selected equation
				select->multiply( 1.0 / select->get_vector()[i] );
				assert( prio != tPrio::kNull );
				const bool sign_postive = ( select->get_relation() == tRelation::kLE ) ? true : false;//x<=a -> x+s==a (positive), x>=a -> x-s==a (neg) with s>=0

				RecoverInfo ri;
				ri.var_idx = i;
				ri.relation = select->get_relation();
				ri.eq_substitution = select->toSparseEquation();//fix
				ri.eq_recover = ri.eq_substitution;//update

				for( auto it = input.begin(); it != input.end(); ++it )
				{
					if( it == select )
						continue;
					DoSubstitution( ri, *it );
				}
				for( auto& e : recoverlist )
					DoSubstitution( ri, e.eq_recover );

				{//do with objective function
					const double coef = input.objectivefunc[i];
					input.objectivefunc.add( -coef, select->get_vector() );
					bias += select->get_rhs() * coef;
					if( prio == tPrio::kLEorGE )
						input.objectivefunc[i] = sign_postive ? -coef : coef;
				}
				//save for recovering i
				ri.eq_recover.get_vector()[i] = sign_postive ? 1 : -1;//recover x from here
				ri.eq_recover.get_vector().reduce();
				recoverlist.emplace_back( ri );
				input.erase( select );
			}
		for( auto& e : input )
			e.get_vector().reduce();//reduce after R transform
		
		//check trivial equation and erase
		const bool hasInvalidEquation = EraseTrivialEquation( input );
		if( hasInvalidEquation )
			return std::make_pair( tError::kInfeasible, 0 );

		//deal with neg rhs
		for( auto& eq : input )
			if( Util::LT( eq.get_rhs(), 0 ) )
				eq.multiply( -1 );

		//calc additional var
		n_slack = 0;
		n_artificial_var = 0;
		n_additional_constraint = 0;
		for( auto& eq : input )
		{
			n_slack += eq.get_relation() != tRelation::kEQ;
			n_artificial_var += eq.get_relation() != tRelation::kLE;
		}
		if constexpr( std::same_as<T, DenseRow> )
		{
			if( n_slack + n_artificial_var > 0 )
				for( auto& eq : input )
					eq.get_vector().resize( GetTotalVar( n ), 0 );
		}
		if constexpr( std::same_as<T, DenseRow> )
			input.objectivefunc.resize( GetTotalVar( n ), 0 );
		const auto isOriginalVar = [n] ( int idx )->bool
		{
			return idx >= 0 && idx < n;
		};
		const auto isSlackVar = [n, this] ( int idx )->bool
		{
			return idx >= n && idx < n + n_slack;
		};
		const auto isArtificialVar = [n, this]( int idx )->bool
		{
			return idx >= n + n_slack && idx < GetTotalVar( n );
		};

		//add var and build idxmap, sub_of
		int idx_slack = n;
		int idx_artificial_var = n + n_slack;
		std::vector<int> idxmap;
		idxmap.reserve( input.size() );

		T sub_of;//min of all artificial var
		if constexpr( std::same_as<T, DenseRow> )
			sub_of.resize( GetTotalVar( n ), 0 );
		if constexpr( std::same_as<T, SparseRow> )
			sub_of.reserve( n + n_slack );
		double sub_ofv = 0;

		for( auto& eq : input )
		{
			if( eq.get_relation() == tRelation::kLE )
			{
				idxmap.emplace_back( idx_slack );
				eq.get_vector()[idx_slack++] = 1;
			}
			else if( eq.get_relation() == tRelation::kEQ )
			{
				idxmap.emplace_back( idx_artificial_var );
				sub_of.add( 1, eq.get_vector() );
				sub_ofv += eq.get_rhs();

				eq.get_vector()[idx_artificial_var++] = 1;
			}
			else if( eq.get_relation() == tRelation::kGE )
			{
				idxmap.emplace_back( idx_artificial_var );
				eq.get_vector()[idx_slack++] = -1;
				sub_of.add( 1, eq.get_vector() );
				sub_ofv += eq.get_rhs();

				eq.get_vector()[idx_artificial_var++] = 1;
			}
		}
		sub_of.reduce();
		sub_of.multiply( -1 );//max form
		
		input.objectivefunc.multiply( -1 );//Z=a+b+c -> -a-b-c+Z=0

		assert( isValidResult( input, idxmap ) );
		//phase 1, refer to (case 3 example is wrong) https://uomustansiriyah.edu.iq/media/lectures/6/6_2022_01_08!08_05_56_PM.pdf
		if( n_artificial_var > 0 )
		{
			iteration = 0;
			auto [ret_code, ret_ofv, sync_ofv] = DoSimplexMethod( sub_of, input.begin(), input.end(), idxmap, &input.objectivefunc );
			phase1_iteration = iteration;
			if( ret_code == tError::kUnbound )
				return std::make_pair( ret_code, 0 );
			assert( ret_code == tError::kOptimum );
			if( !Util::IsZero( ret_ofv - sub_ofv ) )
				return std::make_pair( tError::kInfeasible, 0 );
			assert( isValidResult( input, idxmap ) );

			bias += sync_ofv;

			bool isCleanBasis = true;//no artificial var in basis
			for( int k : idxmap )
				if( isArtificialVar( k ) )
				{
					isCleanBasis = false;
					break;
				}
			if( isCleanBasis )//remove artificial var
			{
				for( auto& eq : input )
				{
					if constexpr( std::same_as<T, DenseRow> )
						std::fill( eq.get_vector().begin() + n + n_slack, eq.get_vector().end(), 0 );
					if constexpr( std::same_as<T, SparseRow> )
						eq.get_vector().erase_if( [isArtificialVar] ( auto& e )->bool
					{
						return isArtificialVar( e.first );
					} );
					assert( eq.get_vector().check() );
					eq.get_vector().reduce();
				}
				//clean OF
				if constexpr( std::same_as<T, DenseRow> )
					std::fill( input.objectivefunc.begin() + n + n_slack, input.objectivefunc.end(), 0 );
				if constexpr( std::same_as<T, SparseRow> )
					input.objectivefunc.erase_if( [isArtificialVar] ( auto& e )->bool
				{
					return isArtificialVar( e.first );
				} );
				input.objectivefunc.reduce();
			}
			else//remove var if sub_of[i] is neg
			{
				const int len = GetTotalVar( n );
				std::vector<bool> toRemove;//ArtificialVar not in idxmap + neg org var
				toRemove.resize( len, false );
				for( int i = 0; i < len; ++i )
				{
					if( isArtificialVar( i ) )
						toRemove[i] = true;
					if(  Util::LT( -sub_of[i], 0 ) )//need neg since it is max form
						toRemove[i] = true;
				}
				for( int k : idxmap )
					if( isArtificialVar( k ) )
						toRemove[k] = false;
				//clean equation
				for( auto& eq : input )
				{
					if constexpr( std::same_as<T, DenseRow> )
						for( int k = 0; k < len; ++k )
							if( toRemove[k] )
								eq.get_vector()[k] = 0;
					if constexpr( std::same_as<T, SparseRow> )
						eq.get_vector().erase_if( [&toRemove] ( auto& e )->bool
					{
						return toRemove[e.first];
					} );
					assert( eq.get_vector().check() );
					eq.get_vector().reduce();
				}
				//clean OF
				if constexpr( std::same_as<T, DenseRow> )
					for( int k = 0; k < len; ++k )
						if( toRemove[k] )
							input.objectivefunc[k] = 0;
				if constexpr( std::same_as<T, SparseRow> )
					input.objectivefunc.erase_if( [&toRemove] ( auto& e )->bool
				{
					return toRemove[e.first];
				} );
				input.objectivefunc.reduce();
			}
		}

		//phase 2
		assert( isValidResult( input, idxmap ) );
		iteration = 0;
		auto [ret_code, ret_ofv, dummy] = use_revised_simplex_method ? 
			DoRevisedSimplexMethod( input.objectivefunc, input.begin(), input.end(), idxmap ) :
			DoSimplexMethod( input.objectivefunc, input.begin(), input.end(), idxmap );
		phase2_iteration = iteration;
		if( ret_code == tError::kUnbound )
			return std::make_pair( ret_code, 0 );
		assert( ret_code == tError::kOptimum );
		assert( isValidResult( input, idxmap ) );
		const double ofv = ret_ofv + bias;

		last_idxmap = idxmap;

		//build org result
		raw_result.clear();
		raw_result.resize( GetTotalVar( n ), 0 );//default 0
		for( int i = 0; auto & eq:input )
		{
			raw_result[idxmap[i]] = eq.get_rhs();//simple proof: at any time, this is a valid solution
			++i;
		}
		//recover shift and substitution
		result.assign( raw_result.begin(), raw_result.begin() + n );
		for( int i = 0; i < n; i++ )
			result[i] += shift[i];
		//assume coef is 1, rhs-result.*vec
		SparseRow sparse_raw_result( raw_result.begin(), raw_result.end() );
		for( auto& e : recoverlist )
		{
			double tmp = e.eq_recover.get_rhs() - e.eq_recover.get_vector().dot( sparse_raw_result );
			result[e.var_idx] = tmp;
		}

		bias = ofv;
		return std::make_pair( tError::kOptimum, SignConvert( ofv, input.isMaximization ) );
	}
	
	//https://opensourc.es/blog/simplex-add/
	template <row_type T>
	std::pair<tError, double> SolveWithNewConstraints( NonStandardFormLinearProgram<T>& input, std::vector<double>& result, const Equation<T>& equation )
	{
		std::vector<Equation<T>> tmp;
		tmp.emplace_back( equation );
		return SolveWithNewConstraints( input, result, tmp.begin(), tmp.end() );
	}

	//assume input is the result from SolveLP
	//new var is NG
	template <row_type T, typename Iter>
	requires std::same_as<Equation<T>, typename std::iterator_traits<Iter>::value_type>
	std::pair<tError, double> SolveWithNewConstraints( NonStandardFormLinearProgram<T>& input, std::vector<double>& result, Iter constraint_st, Iter constraint_ed )
	{
		time.SetTime();
		const int n = (int)input.lb.size();

		//check whether input is match with last run of SolveLP
		if( GetTotalVar( n ) != (int)raw_result.size() )
			return { tError::kInputNotMatchWithLastResult,0 };
		if( input.size() != last_idxmap.size() )
			return { tError::kInputNotMatchWithLastResult,0 };
		if( n != (int)shift.size() )
			return { tError::kInputNotMatchWithLastResult,0 };
		//assume max form
		bool isOptimum = true;
		for( auto e : input.objectivefunc )
		{
			double val = 0;
			if constexpr( std::same_as<T, DenseRow> )
				val = e;
			if constexpr( std::same_as<T, SparseRow> )
				val = e.second;

			if( Util::LT( val, 0 ) )
			{
				isOptimum = false;
				break;
			}
		}
		if( !isOptimum )
			return { tError::kInputNotMatchWithLastResult,0 };

		std::vector<typename NonStandardFormLinearProgram<T>::iterator> idx2basis;
		{
			idx2basis.resize( GetTotalVar( n ), input.end() );
			int i = 0;
			for( auto it = input.begin(); it != input.end(); ++it )
			{
				const int idx = last_idxmap[i];
				idx2basis[idx] = it;
				++i;
			}
		}
		T org_sparse_result( raw_result.begin(), raw_result.end() );
		T sparse_shift( shift.begin(), shift.end() );

		//add constraint
		int from_idx = GetTotalVar( n );
		std::vector<Iter> constraint_list;
		constraint_list.reserve( std::distance( constraint_st, constraint_ed ) );

		//preprocess, even satisfied eq should add (could be used in the future)
		for( auto it = constraint_st; it != constraint_ed; ++it )
		{
			//do transform (shift and substitute)
			if constexpr( std::same_as<T, DenseRow> )
				it->get_vector().resize( GetTotalVar( n ), 0 );
			it->get_rhs() += RemoveLowerBound( it->get_vector(), sparse_shift );
			for( auto& e : recoverlist )
			{
				DoSubstitution( e, *it );
			}

			if constexpr( std::same_as<T, DenseRow> )
				it->get_vector().resize( raw_result.size(), 0 );

			//deal with basis
			{
				const auto backup = it->get_vector();
				int idx = 0;
				for( auto e : backup )
				{
					double val = 0;
					if constexpr( std::same_as<T, DenseRow> )
						val = e;
					if constexpr( std::same_as<T, SparseRow> )
					{
						idx = e.first;
						val = e.second;
					}
					if( !Util::IsZero( val ) && idx2basis[idx] != input.end() )
					{
						assert( Util::EQ( 1, idx2basis[idx]->get_vector()[idx] ) );
						it->add( -val, *idx2basis[idx] );
					}
					++idx;
				}
			}

			if( it->get_relation() != tRelation::kLE )
				it->multiply( -1 );

			it->get_vector().reduce();

			auto [allzero, valid] = it->check();
			if( !valid )
				return std::make_pair( tError::kInfeasible, 0 );
			if( allzero )
				continue;

			constraint_list.emplace_back( it );

			n_additional_constraint += ( it->get_relation() == tRelation::kEQ ) ? 2 : 1;
		}

		//resize and add
		for( auto it : constraint_list )
		{
			if constexpr( std::same_as<T, DenseRow> )
				it->get_vector().resize( GetTotalVar( n ), 0 );

			if( use_perturbation )
				AddPerturbation( it->get_rhs() );

			if( it->get_relation() == tRelation::kEQ )
			{
				auto tmp = *it;
				it->get_relation() = tRelation::kLE;
				tmp.get_relation() = tRelation::kGE;
				tmp.multiply( -1 );

				tmp.get_vector()[from_idx] = 1;
				last_idxmap.emplace_back( from_idx );
				input.emplace_back( std::move( tmp ) );
				++from_idx;
			}
			it->get_vector()[from_idx] = 1;
			last_idxmap.emplace_back( from_idx );
			input.emplace_back( std::move( *it ) );
			++from_idx;
		}

		if constexpr( std::same_as<T, DenseRow> )
		{
			for( auto& e : input )
				e.get_vector().resize( GetTotalVar( n ), 0 );
			input.objectivefunc.resize( GetTotalVar( n ), 0 );
		}
		//do
		iteration = 0;
		auto [ret_code, ret_ofv, dummy] = DoSimplexMethod<T>( input.objectivefunc, input.begin(), input.end(), last_idxmap, nullptr, true );
		phase2_iteration = iteration;
		if( ret_code == tError::kInfeasible )
			return std::make_pair( ret_code, 0 );
		assert( ret_code == tError::kOptimum );
		assert( isValidResult( input, last_idxmap ) );
		for( auto e : input.objectivefunc )
		{
			double val = 0;
			if constexpr( std::same_as<T, DenseRow> )
				val = e;
			if constexpr( std::same_as<T, SparseRow> )
				val = e.second;
			if( Util::LT( val, 0 ) )
				return { tError::kUnbound,0 };
		}
		const double ofv = ret_ofv + bias;

		//build org result
		raw_result.clear();
		raw_result.resize( GetTotalVar( n ), 0 );//default 0
		for( int i = 0; auto & eq:input )
		{
			raw_result[last_idxmap[i]] = eq.get_rhs();//simple proof: at any time, this is a valid solution
			++i;
		}
		//recover shift and substitution
		result.assign( raw_result.begin(), raw_result.begin() + n );
		for( int i = 0; i < n; i++ )
			result[i] += shift[i];
		//assume coef is 1, rhs-result.*vec
		SparseRow sparse_raw_result( raw_result.begin(), raw_result.end() );
		for( auto& e : recoverlist )
		{
			double tmp = e.eq_recover.get_rhs() - e.eq_recover.get_vector().dot( sparse_raw_result );
			result[e.var_idx] = tmp;
		}

		bias = ofv;
		return { tError::kOptimum,SignConvert( ofv,input.isMaximization ) };
	}

private:
	template <typename T>
	bool isValidResult( NonStandardFormLinearProgram<T>& input, const std::vector<int>& idxmap )const
	{
		std::vector<double> result;
		result.resize( input.size(), 0 );
		int i = 0;
		for( auto& eq : input )
		{
			if( idxmap[i] >= (int)result.size() )
				result.resize( idxmap[i] + 1, 0 );
			result[idxmap[i]] = eq.get_rhs();
			++i;
		}
		return input.CheckEquation( result.begin(), result.end() );
	};

	//fail in randomtest_denserow_sparserow
	//https://www.uobabylon.edu.iq/eprints/publication_11_20693_31.pdf
	template <row_type Row, typename T>
	requires std::same_as<Equation<Row>, typename std::iterator_traits<T>::value_type>
	std::tuple<tError, double, double> DoRevisedSimplexMethod( Row& objectivefunc, T st, T ed, std::vector<int>& idxmap, bool dual = false )const
	{
		constexpr int MAX_VECTOR_RESERVED_SIZE = 1000000;//1MB*sizeof(int,double)
		assert( idxmap.size() == std::distance( st, ed ) );
		if( st == ed ) [[unlikely]]
			return std::make_tuple( tError::kOptimum, 0, 0 );
#ifdef _DEBUG
		if( !dual )
		{
			for( auto i = st; i != ed; ++i )
				assert( Util::GE( i->get_rhs(), 0 ) );
		}
#endif
		//do Perturbation
		if( use_perturbation && !dual )
		{
			for( auto it = st; it != ed; ++it )
				AddPerturbation( it->get_rhs() );
		}
		//
		//transform
		//
		int n = objectivefunc.getLength();
		const int m = (int)std::distance( st, ed );
		Equation<SparseRow> compressed_of;
		std::vector<Equation<SparseRow>> basis_matrix;//m*m
		std::vector<Equation<SparseRow>> source_matrix;//input^T (n*m)
		std::vector<typename decltype( source_matrix )::iterator> nonbasis;
		std::vector<bool> isBasis;

		//calc n
		for( auto it = st; it != ed; ++it )
			n = std::max( n, it->get_vector().getLength() );

		//calc isbasis
		isBasis.resize( n, false );
		for( int idx : idxmap )
			isBasis[idx] = true;

		//build matrix
		assert( n > 0 );
		assert( m > 0 );
		basis_matrix.resize( m );
		source_matrix.resize( n );
		const int avg_length = std::min( MAX_VECTOR_RESERVED_SIZE / n + 1, m );//control max memory cost
		assert( avg_length > 0 );
		for( auto& e : source_matrix )
			e.get_vector().reserve( avg_length );//at worst 2*org space

		std::vector<std::pair<int, int>> q;//<var pos, row idx>
		q.reserve( m );
		for( int idx = 0; int x : idxmap )
			q.emplace_back( x, idx++ );
		std::sort( q.begin(), q.end() );
		std::vector<int> varpos2rank;
		varpos2rank.resize( n, -1 );
		for( int idx = 0; auto [x, y] : q )
			varpos2rank[x] = idx++;
		{
			int row_idx = 0;
			SparseRow cache;
			cache.reserve( idxmap.size() );
			for( auto it = st; it != ed; ++it )
			{
				auto cur_basis = basis_matrix.begin() + row_idx;
				cur_basis->get_rhs() = it->get_rhs();
				cache.clear();
				for( auto [idx, val] : it->get_vector().toSparseRow() )
				{
					assert( !Util::IsZero( val ) );
					if( isBasis[idx] )
					{
						assert( varpos2rank[idx] != -1 );
						cache.emplace_back( varpos2rank[idx], val );//copy row
					}
					source_matrix[idx].get_vector().emplace_back( row_idx, val );//transpose
				}
				cur_basis->get_vector().reserve( cache.size() );
				for( auto [idx, val] : cache )
					cur_basis->get_vector().emplace_back( idx, val );
				cur_basis->get_vector().sort();
				assert( cur_basis->get_vector().check() );
				++row_idx;
			}
		}
		compressed_of.get_vector().reserve( m );
		compressed_of.get_rhs() = 0;
		for( auto [idx, val] : objectivefunc.toSparseRow() )
		{
			assert( !Util::IsZero( val ) );
			if( isBasis[idx] )
			{
				assert( varpos2rank[idx] != -1 );
				compressed_of.get_vector().emplace_back( varpos2rank[idx], val );//copy row
			}
			source_matrix[idx].get_rhs() = val;//transpose
		}
		compressed_of.get_vector().sort();
		assert( compressed_of.get_vector().check() );
		//
		//start
		//
		std::vector<double> colval_cache;
		colval_cache.reserve( m );
		while( !isTerminated() )
		{
			++iteration;

			int col = -1;
			int row = -1;
			double col_val = 0;
			double cell_val = 0;
			auto org_vec = source_matrix.end();
			auto base = basis_matrix.end();
			if( !dual )
			{
				//FindPivotColumn
				for( int idx = 0; auto & eq : source_matrix )
				{
					if( !isBasis[idx] )
					{
						double val = compressed_of.get_vector().dot( eq.get_vector() );
						val += eq.get_rhs();
						if( col == -1 || Util::LT( val, col_val ) )
						{
							col = idx;
							col_val = val;
						}
					}
					++idx;
				}
				if( col == -1 || Util::GE( col_val, 0 ) )
					break;//done
				org_vec = source_matrix.begin() + col;

				//FindPivotRow
				double tmp_rhs = 0;
				colval_cache.clear();
				for( int idx = 0; auto & eq : basis_matrix )
				{
					const double val = org_vec->get_vector().dot( eq.get_vector() );
					colval_cache.emplace_back( val );
					//rhs/val < best
					if( Util::GT( val, 0 ) && ( row == -1 || eq.get_rhs() * cell_val < val * tmp_rhs ) )
					{
						row = idx;
						tmp_rhs = eq.get_rhs();
						cell_val = val;
					}
					++idx;
				}
				if( row == -1 )
					return std::tuple( tError::kUnbound, 0, 0 );
				base = basis_matrix.begin() + row;
				assert( Util::GT( cell_val, 0 ) );
			}
			else
			{
				//todo dual
			}
			assert( row != -1 && col != -1 && base != basis_matrix.end() );
			
			base->multiply( 1.0 / cell_val );//normalize
			assert( Util::EQ( 1, colval_cache[row] / cell_val ) );
			
			const double coef = -( org_vec->get_vector().dot( compressed_of.get_vector() ) + org_vec->get_rhs() );
			compressed_of.add( coef, *base );

			isBasis[idxmap[row]] = false;
			idxmap[row] = col;
			isBasis[col] = true;

			//remove row if non-zero <i,col>
			auto val_ptr = colval_cache.begin();
			for( auto i = basis_matrix.begin(); i != basis_matrix.end(); ++i )
			{
				const double val = *( val_ptr++ );
				if( i == base || Util::IsZero( val ) )
					continue;
				i->add( -val, *base );
			}
		}
		//set result
		{
			assert( m == (int)idxmap.size() );

			SparseRow cache;
			int row_idx = 0;
			for( auto it = st; it != ed; ++it )
			{
				auto cur_basis = basis_matrix.begin() + row_idx;
				
				cache.clear();
				for( int idx = 0; auto & eq : source_matrix )
				{
					cache.emplace_back( idx, cur_basis->get_vector().dot( eq.get_vector() ) );
					++idx;
				}
				assert( cache.check() );
				if constexpr( std::same_as<Row, DenseRow> )
					it->get_vector() = cache.toDenseRow();
				if constexpr( std::same_as<Row, SparseRow> )
					it->get_vector() = cache;
				it->get_rhs() = cur_basis->get_rhs();
				++row_idx;
			}
			//of
			cache.clear();
			for( int idx = 0; auto & eq : source_matrix )
			{
				const double val = compressed_of.get_vector().dot( eq.get_vector() ) + eq.get_rhs();
				if( !Util::IsZero( val ) )
				{
					cache.emplace_back( idx, val );
				}
				++idx;
			}
			assert( cache.check() );
			if constexpr( std::same_as<Row, DenseRow> )
				objectivefunc = cache.toDenseRow();
			if constexpr( std::same_as<Row, SparseRow> )
				objectivefunc = cache;
		}

		return { tError::kOptimum,compressed_of.get_rhs(),0 };
	}

	//Assume Maximization, non-negative variable, equation, non-negative rhs
	//return <status,ofv,sync ofv> and idxmap (base var idx idxmap[i] on constraint i)
	//modify equation and OF, sync does the same transform as OF
	template <row_type Row, typename T>
	requires std::same_as<Equation<Row>, typename std::iterator_traits<T>::value_type>
	std::tuple<tError, double, double> DoSimplexMethod( Row& objectivefunc, T st, T ed, std::vector<int>& idxmap, Row* sync = nullptr, bool dual = false )const
	{
		assert( idxmap.size() == std::distance( st, ed ) );
		if( st == ed ) [[unlikely]]
			return std::make_tuple( tError::kOptimum, 0, 0 );
#ifdef _DEBUG
		if( !dual )
		{
			for( auto i = st; i != ed; ++i )
				assert( Util::GE( i->get_rhs(), 0 ) );
		}
#endif
		//do Perturbation
		if( use_perturbation && !dual )
		{
			for( auto it = st; it != ed; ++it )
				AddPerturbation( it->get_rhs() );
		}

		double ofv = 0;
		double sync_ofv = 0;

		while( !isTerminated() )
		{
			++iteration;

			int col = -1;
			int row = -1;
			double col_val = 0;
			double cell_val = 0;
			auto base = ed;
			if( !dual )
			{
				std::tie( col, col_val ) = FindPivotColumn( objectivefunc );
				if( Util::GE( col_val, 0 ) )
					break;//done

				std::tie( row, cell_val, base ) = FindPivotRow<Row>( col, st, ed );
				if( row == -1 )
					return std::tuple( tError::kUnbound, 0, 0 );
				assert( Util::GT( cell_val, 0 ) );
			}
			else
			{
				double rhs = 0;
				std::tie( row, rhs, base ) = FindDualPivotRow<Row>( st, ed );
				if( Util::GE( rhs, 0 ) )
					break;

				std::tie( col, cell_val ) = FindDualPivotColumn( objectivefunc, *base );
				if( col == -1 )
					return std::tuple( tError::kInfeasible, 0, 0 );

				col_val = objectivefunc[col];
			}
			assert( row != -1 && col != -1 && base != ed );
			
			base->multiply( 1.0 / cell_val );//normalize
			assert( Util::EQ( 1, base->get_vector()[col] ) );

			idxmap[row] = col;

			//remove row if non-zero <i,col>
			for( auto i = st; i != ed; ++i )
			{
				if( i == base ) [[unlikely]]
					continue;
				const double val = i->get_vector()[col];
				if( Util::IsZero( val ) )
					continue;
				i->add( -val, *base );
				assert( Util::IsZero( i->get_vector()[col] ) );
			}
			objectivefunc.add( -col_val, base->get_vector() );
			assert( Util::IsZero( objectivefunc[col] ) );
			ofv += -col_val * base->get_rhs();

			//refer to https://webspace.maths.qmul.ac.uk/felix.fischer/teaching/opt/notes/notes8.pdf
			if( sync )
			{
				const double coef = ( *sync )[col];
				if( !Util::IsZero( coef ) )
				{
					sync->add( -coef, base->get_vector() );
					assert( Util::IsZero( ( *sync )[col] ) );
					sync_ofv += -coef * base->get_rhs();
				}
			}
		}

		return std::make_tuple( tError::kOptimum, ofv, sync_ofv );
	}

	//find the most negative value from objective function, return <col,value>
	//other rules https://people.orie.cornell.edu/dpw/orie6300/Lectures/lec13.pdf
	template <row_type Row>
	std::pair<int, double> FindPivotColumn( const Row& objectivefunc )const
	{
		if constexpr( std::same_as<Row, DenseRow> )
		{
			assert( !objectivefunc.empty() );
			auto elm = std::min_element( objectivefunc.begin(), objectivefunc.end() );
			assert( elm != objectivefunc.end() );
			const int dis = static_cast<int>( std::distance( objectivefunc.begin(), elm ) );
			return std::make_pair( dis, *elm );
		}
		if constexpr( std::same_as<Row, SparseRow> )
		{
			if( objectivefunc.empty() ) [[unlikely]]
				return { 0,0 };
			auto elm = std::min_element( objectivefunc.begin(), objectivefunc.end(), [] ( auto& a, auto& b )->bool
			{
				return a.second < b.second;//ignore eps in min
			} );
			assert( elm != objectivefunc.end() );
			return *elm;
		}
		assert( 0 );
		return { 0,0 };
	}
	//find minimum positive rhs[i]/mat[i][col] with mat[i][col]>0, return <row,val,iter>
	template <row_type Row, typename T>
	requires std::same_as<Equation<Row>, typename std::iterator_traits<T>::value_type>
	std::tuple<int, double, T> FindPivotRow( const int col, T st, T ed )const
	{
		assert( st != ed );
		std::vector<double> tmp;
		T ret = ed;
		int row = -1;
		double best_rhs = 0;
		double best_val = 0;
		int idx = 0;
		for( T cur = st; cur != ed; ++cur )
		{
			double val = cur->get_vector()[col];
			//rhs[i]/val[i] < rhs[best]/val[best]
			if( Util::GT( val, 0 ) && ( row == -1 || cur->get_rhs() * best_val < best_rhs * val ) )
			{
				ret = cur;
				row = idx;
				best_rhs = cur->get_rhs();
				best_val = val;
				assert( Util::GE( best_rhs, 0 ) );
			}
			++idx;
		}
		return std::make_tuple( row, best_val, ret );
	}
	
	//min of rhs, return <row,rhs,iterator>
	template <row_type Row, typename T>
	requires std::same_as<Equation<Row>, typename std::iterator_traits<T>::value_type>
	std::tuple<int, double, T> FindDualPivotRow( T st, T ed )const
	{
		assert( st != ed );
		auto elm = std::min_element( st, ed, [] ( auto& a, auto& b )->bool
		{
			return a.get_rhs() < b.get_rhs();
		} );
		assert( elm != ed );
		const int row = (int)std::distance( st, elm );
		return std::make_tuple( row, elm->get_rhs(), elm );
	}

	//return min of abs(of[i]/row[i]), <col,val>
	template <row_type Row>
	std::pair<int, double> FindDualPivotColumn( const Row& objectivefunc, const Equation<Row> row )const
	{
		int col = -1;
		double best_coef = 0;
		double best_coef_of = 0;

		for( int idx = 0; auto e : row.get_vector() )
		{
			double val = 0;
			if constexpr( std::same_as<Row, DenseRow> )
				val = e;
			if constexpr( std::same_as<Row, SparseRow> )
			{
				idx = e.first;
				val = e.second;
			}

			//ofv[idx]/val < best_coef_of / best_coef,  ignore sign
			const double of_val = objectivefunc[idx];
			if( Util::LT( val, 0 ) && ( col == -1 || fabs( of_val * best_coef ) < fabs( best_coef_of * val ) ) )
			{
				col = idx;
				best_coef = val;
				best_coef_of = of_val;
			}

			++idx;
		}
		return std::make_pair( col, best_coef );
	}

	void AddPerturbation( double& val )const
	{
		static std::uniform_real_distribution<double> RandPerturbation( 0.01, 1 );
		static_assert( sizeof( double ) >= 8 );
		static_assert( Util::eps > PerturbationEPS );
		val = std::fma( RandPerturbation( rng ), PerturbationEPS, val );
	}

	//remove trivial equation (always true) and check if there is invalid equation (always false)
	template<row_type T>
	bool EraseTrivialEquation( NonStandardFormLinearProgram<T>& input )const
	{
		bool hasInvalidEquation = false;
		std::erase_if( input, [&hasInvalidEquation] ( auto& eq )->bool
		{
			auto [allzero, valid] = eq.check();
			if( allzero )
			{
				if( !valid )
					hasInvalidEquation = true;
				return true;
			}
			return false;
		} );
		return hasInvalidEquation;
	}

	constexpr int GetTotalVar( int n )const noexcept
	{
		return n + n_slack + n_artificial_var + n_additional_constraint;
	}
	constexpr double SignConvert( double val, bool isMaximize )const noexcept
	{
		return isMaximize ? val : -val;
	}
	//x>=1 -> y=x-1 && y>=0
	template <row_type T>
	double RemoveLowerBound( const T& vec, const T& lb )const
	{
		return -vec.dot( lb );
	}
	template <row_type T>
	void DoSubstitution( const RecoverInfo& select, Equation<T>& target )const
	{
		const double coef = target.get_vector()[select.var_idx];
		if( Util::IsZero( coef ) )
			return;
		target.add( -coef, select.eq_substitution );

		if( select.relation != tRelation::kEQ )
			target.get_vector()[select.var_idx] = ( select.relation == tRelation::kLE ) ? -coef : coef;//use i as new var, this is new coef
	}
};

}