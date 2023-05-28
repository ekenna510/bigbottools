# bigbottools Ros2 version
**testencoder** - sends a speed m/s (parameter speed) cmd_vel linear.x for so many iternation (parameter loops). Then sends 0 to stop

ros2 run  bigbottools testencoder --ros-args -p loops:=200 -p speed:=0.2



**Subpwm** - subscribe to pwm
**pubpwm** - publish pwm
**teleop** - a teleop program
**joyteleop** - Joy stick version
**botconfig,sensordata,serial,slaveparse** - sensor and communication modules
