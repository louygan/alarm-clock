# usage
# aa=$(script_path)
script_path()
{
  CURDIR=$PWD
  SCRIPT_PATH="${BASH_SOURCE[0]}";
  while([ -h "${SCRIPT_PATH}" ]); do
    cd "`dirname "${SCRIPT_PATH}"`"
    SCRIPT_PATH="$(readlink "`basename "${SCRIPT_PATH}"`")";
  done
  cd "`dirname "${SCRIPT_PATH}"`" > /dev/null
  SCRIPT_PATH="`pwd`";
  cd $CURDIR
  echo "$SCRIPT_PATH"
}

DIR=$PWD

cd $(script_path)


# ./config.h:5:#define ALARM_CLOCK_RUN_IN_SOURCE_TREE 1

# use Dockerfile in ./
# other Dockerfile maybe found in docker directory
docker build docker  -t alarm-clock:build

docker run -it -v$PWD:$PWD alarm-clock:build sh -c "cd $PWD; make"

cd $DIR
