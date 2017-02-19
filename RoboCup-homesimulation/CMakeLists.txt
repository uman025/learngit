PROJECT(dreamwing)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g")


FILE(GLOB dreamwing_SRCS
	object.hpp
	object.cpp
	instruction.hpp
	instruction.cpp
	nature.hpp
	nature.cpp
    Robot.hpp
	plan_error_final.cpp
    Robot.cpp
    main.cpp
)
SOURCE_GROUP("" FILES ${dreamwing_SRCS})

ADD_EXECUTABLE(dreamwing ${dreamwing_SRCS})

ADD_DEPENDENCIES(dreamwing frame utility)
TARGET_LINK_LIBRARIES(dreamwing frame utility ${Boost_LIBRARIES})
