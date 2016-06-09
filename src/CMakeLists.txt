include_directories(../portaLib)
link_directories(~/notDebian/ob/lib/)

add_library(
  wCommon STATIC
  Counted.h SmartPtr.h
  Essential.h
  DIGenerator.h DIGenerator.cpp
  ExpressionList.h ExpressionList.cpp
  Expression.h Expression.cpp
  Util.h Util.cpp
  LogStream.h LogStream.cpp
  Evaluator.h
  SEvaluator.h SEvaluator.cpp
  ZerosEvaluator.h ZerosEvaluator.cpp
  OrderEvaluator.h OrderEvaluator.cpp
  RHSEvaluator.h RHSEvaluator.cpp
  IntList.h IntList.cpp
  StackTrace.cpp
  mkpath.cpp
 )


#-----------
add_executable(
  wMat
  WriteMat.cpp
  )

target_link_libraries(wMat wCommon)
set_property(TARGET wMat PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wFilter
  Filter.cpp
)

target_link_libraries(wFilter wCommon)
set_property(TARGET wFilter PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wPoly
  Symmetries.h Symmetries.cpp
  Poly.cpp
)

target_link_libraries(wPoly wCommon)
set_property(TARGET wPoly PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wSelect
  Select.cpp
)

target_link_libraries(wSelect wCommon)
set_property(TARGET wSelect PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wTex
  Tex.cpp
)
target_link_libraries(wTex wCommon)
set_property(TARGET wTex PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wVerify
  Verify.cpp
)

target_link_libraries(wVerify wCommon)
set_property(TARGET wVerify PROPERTY LINK_SEARCH_START_STATIC 1)
#-----------
add_executable(
  wTests
  Tests.cpp
)

target_link_libraries(wTests wCommon)
set_property(TARGET wTests PROPERTY LINK_SEARCH_START_STATIC 1)

add_executable(
  wCut
  Cut.cpp
  )

target_link_libraries(wCut wCommon)
set_property(TARGET wCut PROPERTY LINK_SEARCH_START_STATIC 1)

# eof