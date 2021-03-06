#pragma once

// boost
#define BOOST_NO_AUTO_PTR
#include <boost/shared_ptr.hpp>

// Eigen
#include <Eigen/Eigen>

// Helpers
#include <Helpers.h>

// Devil's Details' bullshit
#include <DX_PATCH.h>

template<int n> struct InsertPadding
{
    INSERT_PADDING(n);
};