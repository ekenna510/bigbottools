cmake_minimum_required(VERSION 3.5)
project(bigbottools)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wextra -Wpedantic )
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(joy REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
#find_package(motorcontrol REQUIRED)

include_directories(
  include
)


#target_link_libraries(${PROJECT_NAME}_lib)

#add_executable(dosquare src/dosquare.cpp  )
#ament_target_dependencies(dosquare rclcpp std_msgs geometry_msgs tf2 tf2_msgs tf2_ros sensor_msgs) 

add_executable(fake_scan src/fake_scan.cpp  )
ament_target_dependencies(fake_scan rclcpp sensor_msgs ) 

add_executable(joyteleop src/joyteleop.cpp  )
ament_target_dependencies(joyteleop rclcpp geometry_msgs sensor_msgs ) 

add_executable(testpubodom src/testpubodom.cpp  )
ament_target_dependencies(testpubodom rclcpp std_msgs geometry_msgs tf2 tf2_msgs tf2_ros nav_msgs) 

add_executable(testbotconfig src/testbotconfig.cpp src/botconfig.cpp src/SlaveParse.cpp)
ament_target_dependencies(testbotconfig rclcpp ) 

add_executable(testsensordata src/testsensordata.cpp  src/botconfig.cpp src/SlaveParse.cpp src/sensordata.cpp)
ament_target_dependencies(testsensordata rclcpp sensor_msgs) 

add_executable(testserial src/testserial.cpp src/serial.cpp )
ament_target_dependencies(testserial rclcpp ) 

add_executable(teleop src/teleop.cpp  )
ament_target_dependencies(teleop rclcpp std_msgs geometry_msgs) 

add_executable(pubpwm src/pubpwm.cpp  )
ament_target_dependencies(pubpwm rclcpp std_msgs geometry_msgs ) 

add_executable(subpwm src/subpwm.cpp  )
ament_target_dependencies(subpwm rclcpp std_msgs geometry_msgs ) 

#add_executable(testencoder src/testencoder.cpp  )
#ament_target_dependencies(testencoder rclcpp std_msgs geometry_msgs nav_msgs) 


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()


################################################################################
# Install
################################################################################
install(DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS fake_scan DESTINATION lib/${PROJECT_NAME} )
install(TARGETS joyteleop DESTINATION lib/${PROJECT_NAME} )
install(TARGETS testpubodom DESTINATION lib/${PROJECT_NAME} )
install(TARGETS testbotconfig DESTINATION lib/${PROJECT_NAME} )
install(TARGETS testsensordata DESTINATION lib/${PROJECT_NAME} )
install(TARGETS testserial DESTINATION lib/${PROJECT_NAME} )
install(TARGETS teleop DESTINATION lib/${PROJECT_NAME} )
install(TARGETS pubpwm DESTINATION lib/${PROJECT_NAME} )
install(TARGETS subpwm DESTINATION lib/${PROJECT_NAME} )
#install(TARGETS testencoder DESTINATION lib/${PROJECT_NAME} )


################################################################################
# Macro for ament package
################################################################################


ament_export_include_directories(include)
ament_package()
