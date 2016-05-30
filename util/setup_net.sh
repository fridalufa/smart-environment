#! /bin/sh

# Check if tmux is installed
if ! [ -x "$(command -v tmux)" ]; then
  echo "tmux is not installed.\nPlease install tmux if you want to use this script."
  exit 1
fi


# determine paths of the smart environment project and RIOT
SMART_ENVIRONMENT_PATH=$(readlink -f "$(dirname $(readlink -f $0))/..")
RIOT_PATH=$(readlink -f "$SMART_ENVIRONMENT_PATH/../RIOT")

# tapsetup
TAPSETUP="$RIOT_PATH/dist/tools/tapsetup/tapsetup"

if ! [ -e $TAPSETUP ]; then
    echo "tapsetup is not present. Please check your project setup."
    exit 1
fi

$TAPSETUP -d
$TAPSETUP -c 3

# make the smart environment project
cd $SMART_ENVIRONMENT_PATH
make all

# start sessions with the first session running the root node
tmux new-session -d -s smartenv
tmux rename-window 'Smart Environment'
tmux select-window -t smartenv:0
tmux send-keys "cd $SMART_ENVIRONMENT_PATH" 'C-m' 'make term PORT=tap0 MODE=root' 'C-m'

# create and init second session
tmux split-window -h -t 0
tmux select-window -t smartenv:1
tmux send-keys "cd $SMART_ENVIRONMENT_PATH" 'C-m' 'make term PORT=tap1' 'C-m'

# create and init third session
tmux split-window -h -t 0
tmux select-window -t smartenv:2
tmux send-keys "cd $SMART_ENVIRONMENT_PATH" 'C-m' 'make term PORT=tap2' 'C-m'
tmux select-layout even-horizontal

# finally attach to the tmux session
tmux -2 attach-session -t smartenv