load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_binary",
    "envoy_cc_test",
    "envoy_contrib_package",
)
load(
    "//contrib:all_contrib_extensions.bzl",
    "envoy_all_contrib_extensions",
)

licenses(["notice"])  # Apache 2

envoy_contrib_package()

alias(
    name = "envoy",
    actual = ":envoy-static",
)

envoy_cc_binary(
    name = "envoy-static",
    stamped = True,
    deps = ["//source/exe:envoy_main_entry_lib"] + envoy_all_contrib_extensions(),
)

envoy_cc_test(
    name = "example_configs_test",
    size = "large",
    data = [
        "//examples:contrib_configs",
        "//test/config_test:example_configs_test_setup.sh",
    ],
    env = {
        "EXAMPLE_CONFIGS_TAR_PATH": "envoy/examples/example_configs.tar",
        "DISABLE_TEST_MERGE": "true",
    },
    deps = [
        "//test/config_test:example_configs_test_lib",
    ] + envoy_all_contrib_extensions(),
)
