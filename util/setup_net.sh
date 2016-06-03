#! /bin/bash

# number of actors
ACTORS=2
# number of sensors
SENSORS=2

# parse arguments
while [[ $# > 1 ]]
do
opt="$1"

case $opt in
    -a|--actors)
    ACTORS="$2"
    shift
    ;;
    -s|--sensors)
    SENSORS="$2"
    shift
    ;;
    *)
    echo "Unknown option: $1"
    exit 1
    ;;
esac
shift
done

TOTAL_NODES=$(($ACTORS + $SENSORS))

# Check if tmux is installed
if ! [ -x "$(command -v tmux)" ]; then
  echo "tmux is not installed.\nPlease install tmux if you want to use this script."
  exit 1
fi

# determine paths of the smart environment project and RIOT
SMART_ENVIRONMENT_PATH=$(readlink -f "$(dirname $(readlink -f $0))/..")
SMART_ENV_ACTOR_PATH=$(readlink -f $SMART_ENVIRONMENT_PATH/actor)
SMART_ENV_SENSOR_PATH=$(readlink -f $SMART_ENVIRONMENT_PATH/sensor)
RIOT_PATH=$(readlink -f "$SMART_ENVIRONMENT_PATH/../RIOT")

# tapsetup
TAPSETUP="$RIOT_PATH/dist/tools/tapsetup/tapsetup"

if ! [ -e $TAPSETUP ]; then
    echo "tapsetup is not present. Please check your project setup."
    exit 1
fi

$TAPSETUP -d
$TAPSETUP -c $TOTAL_NODES

# make the actors
cd $SMART_ENV_ACTOR_PATH
make all
# and the sensors
cd $SMART_ENV_SENSOR_PATH
make all

tmux new-session -d -s smartenv
tmux rename-window 'Smart Environment'
# start actor sessions
for i in $(seq 0 $(($ACTORS-1)))
do
    if [ $i -gt 0 ]; then
        tmux split-window -h -t $(($i-1))
    fi
    tmux select-window -t smartenv:$i
    tmux send-keys "cd $SMART_ENV_ACTOR_PATH" 'C-m' "make term PORT=tap$i" 'C-m'
done
# start sensor sessions
for j in $(seq $ACTORS $(($TOTAL_NODES-1)))
do
    if [ $j -gt 0 ]; then
        tmux split-window -h -t $(($j-1))
    fi
    tmux select-window -t smartenv:$j
    tmux send-keys "cd $SMART_ENV_SENSOR_PATH" 'C-m' "make term PORT=tap$j" 'C-m'
done

# arrange sessions horizontally
tmux select-layout even-horizontal

# finally attach to the tmux session
tmux -2 attach-session -t smartenv