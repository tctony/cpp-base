test_cases="simple worsest"
for tc in $test_cases
do
bazel run //zlog/benchmark:main --//zlog/benchmark:log_case=$tc 2>/dev/null
bazel run //zlog/benchmark:main --//zlog/benchmark:log_case=$tc --//zlog:light_weight_string=1 2>/dev/null
done

