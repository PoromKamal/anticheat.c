# require : either -v or -g as command line arguments
if [ $# -eq 0 ]
then
    echo "No arguments supplied"
    exit 1
fi

if [ $1 == "-setup" ] # Create a large directory to test on
then
    echo "Setting up time bench"
    time ./anticheat-dev doesntMatter -t
elif [ $1 == "-v" ]
then
    echo "Running time benchmark for verifying files."
    time ./anticheat-dev benchmarkTests -v
elif [ $1 == "-g" ]
then
    echo "Running time benchmark for generating files."
    time ./anticheat-dev benchmarkTests -g
else
    echo "Invalid argument"
    exit 1
fi