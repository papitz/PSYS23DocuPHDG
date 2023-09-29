add_test([=[HeatMatrixTest.ConstructorAndDimensions]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.ConstructorAndDimensions]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.ConstructorAndDimensions]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatMatrixTest.SetAndGetTemp]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.SetAndGetTemp]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.SetAndGetTemp]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatMatrixTest.AccumulateAllTemps]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.AccumulateAllTemps]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.AccumulateAllTemps]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatMatrixTest.CheckForConversion]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.CheckForConversion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.CheckForConversion]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatMatrixTest.setTempInArea]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.setTempInArea]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.setTempInArea]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatMatrixTest.SliceTest]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatMatrixTest.SliceTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatMatrixTest.SliceTest]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatFunctionsTest.CalculateNextTempOfTile]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatFunctionsTest.CalculateNextTempOfTile]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatFunctionsTest.CalculateNextTempOfTile]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatFunctionsTest.ZeroOrRim]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatFunctionsTest.ZeroOrRim]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatFunctionsTest.ZeroOrRim]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[HeatFunctionsTest.CalculateHeatMatrix]=]  /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test/t_HeatMatrix [==[--gtest_filter=HeatFunctionsTest.CalculateHeatMatrix]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[HeatFunctionsTest.CalculateHeatMatrix]=]  PROPERTIES WORKING_DIRECTORY /home/paul/Uni/PSYS/psys23-Belegarbeit-DennisGottfried-PaulHoeppner/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  t_HeatMatrix_TESTS HeatMatrixTest.ConstructorAndDimensions HeatMatrixTest.SetAndGetTemp HeatMatrixTest.AccumulateAllTemps HeatMatrixTest.CheckForConversion HeatMatrixTest.setTempInArea HeatMatrixTest.SliceTest HeatFunctionsTest.CalculateNextTempOfTile HeatFunctionsTest.ZeroOrRim HeatFunctionsTest.CalculateHeatMatrix)