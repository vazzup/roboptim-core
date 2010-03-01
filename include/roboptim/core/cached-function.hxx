// Copyright (C) 2010 by Thomas Moulard, AIST, CNRS, INRIA.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ROBOPTIM_CORE_CACHED_FUNCTION_HXX
# define ROBOPTIM_CORE_CACHED_FUNCTION_HXX
# include <boost/format.hpp>

namespace roboptim
{
  namespace
  {
    template <typename T>
    std::string cachedFunctionName (const T& fct);

    template <typename T>
    std::string cachedFunctionName (const T& fct)
    {
      boost::format fmt ("%1% (cached)");
      fmt % fct.getName ();
      return fmt.str ();
    }

    template <typename T>
    struct derivativeSize;

    template <>
    struct derivativeSize<Function>
    {
      static const unsigned int value = 0;
    };

    template <>
    struct derivativeSize<DerivableFunction>
    {
      static const unsigned int value = 1;
    };

    template <>
    struct derivativeSize<TwiceDerivableFunction>
    {
      static const unsigned int value = 2;
    };

    template <unsigned N>
    struct derivativeSize<NTimesDerivableFunction<N> >
    {
      static const unsigned int value = 3;
    };

  } // end of anonymous namespace.

  template <typename T>
  CachedFunction<T>::CachedFunction (boost::shared_ptr<const T> fct) throw ()
    : T (fct->inputSize (), fct->outputSize (), cachedFunctionName (*fct)),
      function_ (fct),
      cache_ (derivativeSize<T>::value),
      gradientCache_ (fct->outputSize ()),
      hessianCache_ (fct->outputSize ())
  {
  }

  template <typename T>
  CachedFunction<T>::~CachedFunction () throw ()
  {
  }

  template <typename T>
  void
  CachedFunction<T>::reset () throw ()
  {
    cache_.clear ();
    gradientCache_.clear ();
    hessianCache_.clear ();
  }


  template <typename T>
  void
  CachedFunction<T>::impl_compute (result_t& result,
				   const argument_t& argument)
    const throw ()
  {
    functionCache_t::const_iterator it = cache_[0].find (argument);
    if (it != cache_[0].end ())
      {
	std::cout << "cached" << std::endl;
	result = it->second;
	return;
      }
    std::cout << "not cached" << std::endl;
    (*function_) (result, argument);
    cache_[0][argument] = result;
  }


  template <>
  void
  CachedFunction<Function>::impl_gradient (gradient_t&, const argument_t&, size_type)
    const throw ()
  {
    assert (0);
  }

  template <typename T>
  void
  CachedFunction<T>::impl_gradient (gradient_t& gradient,
				    const argument_t& argument,
				    size_type functionId)
    const throw ()
  {
    functionCache_t::const_iterator it =
      gradientCache_[functionId].find (argument);
    if (it != gradientCache_[functionId].end ())
      {
	gradient = it->second;
	return;
      }
    function_->gradient (gradient, argument, functionId);
    gradientCache_[functionId][argument] = gradient;
  }




  template <>
  void
  CachedFunction<Function>::impl_hessian
  (hessian_t&, const argument_t&, size_type) const throw ()
  {
    assert (0);
  }

  template <>
  void
  CachedFunction<DerivableFunction>::impl_hessian
  (hessian_t&, const argument_t&, size_type) const throw ()
  {
    assert (0);
  }



  template <typename T>
  void
  CachedFunction<T>::impl_hessian (hessian_t& hessian,
  				   const argument_t& argument,
  				   size_type functionId)
    const throw ()
  {
    functionCache_t::const_iterator it =
      hessianCache_[functionId].find (argument);
    if (it != hessianCache_[functionId].end ())
      {
	hessian = it->second;
	return;
      }
    function_->hessian (hessian, argument, functionId);
    hessianCache_[functionId][argument] = hessian;
  }


  template <>
  void
  CachedFunction<Function>::impl_derivative
  (gradient_t&, double, size_type) const throw ()
  {
    assert (0);
  }

  template <>
  void
  CachedFunction<DerivableFunction>::impl_derivative
  (gradient_t&, double, size_type) const throw ()
  {
    assert (0);
  }

  template <>
  void
  CachedFunction<TwiceDerivableFunction>::impl_derivative
  (gradient_t&, double, size_type) const throw ()
  {
    assert (0);
  }

  template <typename T>
  void
  CachedFunction<T>::impl_derivative (gradient_t& derivative,
  				      double argument,
  				      size_type order)
    const throw ()
  {
    vector_t x (1);
    x[0] = argument;
    functionCache_t::const_iterator it = cache_[order].find (x);
    if (it != cache_[order].end ())
      {
	derivative = it->second;
	return;
      }
    function_->derivative (derivative, x, order);
    cache_[order][x] = derivative;
  }

} // end of namespace roboptim

#endif //! ROBOPTIM_CORE_CACHED_FUNCTION_HXX
