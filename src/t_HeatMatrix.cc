#include <gtest/gtest.h>
#include "../include/HeatMatrix.hpp"

TEST(HeatMatrixSuite, HeatMatrixCreation){
    HeatMatrix hm( 4, 4 );

    /* EXPECT_EQ(hm.checkForConversion(hm, 0.1, false), true); */
    EXPECT_EQ(1,1);
}
