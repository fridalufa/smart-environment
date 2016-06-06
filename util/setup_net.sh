#! /bin/bash

# number of actors
ACTORS=2
# number of sensors
SENSORS=2

usage() {
    PROGRAM=$(basename $0)
    echo "Creates a net of sensors and actors running on RIOT native nodes." >&2
    echo "" >&2
    echo "Usage: $PROGRAM [options]" >&2
    echo "" >&2
    echo "Default behaviour:"
    echo "    If no options are given, $ACTORS actors and $SENSORS sensors will be created." >&2
    echo "" >&2
    echo "Options" >&2
    echo "    -a <num>, --actors <num>:   Create <num> actors" >&2
    echo "    -s <num, --sensors <num>:   Create <num> sensors" >&2
    echo "    -h, --help:                 Print this text" >&2
}


# parse arguments
while [[ $# > 0 ]]
do

opt="$1"

case $opt in

    -a|--actors)
    case "$2" in
        ""|*[!0-9]*)
            usage
            exit 1 ;;
        *)
            ACTORS="$2"
            shift ;;
    esac ;;

    -s|--sensors)
        case "$2" in
        ""|*[!0-9]*)
            usage
            exit 1 ;;
        *)
            SENSORS="$2"
            shift ;;
    esac ;;

    -h|--help)
    usage
    exit ;;

    *)
    usage
    exit 1 ;;
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
    tmux send-keys "cd $SMART_ENV_ACTOR_PATH" 'C-m' "make term PORT=tap$i" 'C-m'
    if [ $i -eq 0 ]; then
        tmux send-keys 'mkroot' 'C-m'
    fi
done

# start sensor sessions
for j in $(seq $ACTORS $(($TOTAL_NODES-1)))
do
    if [ $j -gt 0 ]; then
        tmux split-window -h -t $(($j-1))
    fi
    tmux send-keys "cd $SMART_ENV_SENSOR_PATH" 'C-m' "make term PORT=tap$j" 'C-m'
done

# arrange sessions horizontally
tmux select-layout even-horizontal

# finally attach to the tmux session
tmux -2 attach-session -t smartenv