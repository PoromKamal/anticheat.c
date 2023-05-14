# require : either -v or -g as command line arguments
if [ $# -eq 0 ]
then
    echo "No arguments supplied"
    exit 1
fi

if [ $1 == "-v" ]
then
    echo "Running time benchmark for verifying files."
    time ./anticheat-dev testGame -v
elif [ $1 == "-g" ]
then
    echo "Running time benchmark for generating files."
    time ./anticheat-dev testGame -g
else
    echo "Invalid argument"
    exit 1
fi