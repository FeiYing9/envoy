load("@base_pip3//:requirements.bzl", "requirement")
load("//bazel:envoy_build_system.bzl", "envoy_package")
load("//tools/base:envoy_python.bzl", "envoy_py_library")

licenses(["notice"])  # Apache 2

envoy_package()

envoy_py_library("tools.base.abstract")

envoy_py_library(
    "tools.base.aio",
    deps = [
        ":functional",
    ],
)

envoy_py_library(
    "tools.base.checker",
    deps = [
        ":runner",
    ],
)

envoy_py_library("tools.base.functional")

envoy_py_library(
    "tools.base.runner",
    deps = [
        requirement("colorama"),
        requirement("coloredlogs"),
        requirement("frozendict"),
        requirement("verboselogs"),
    ],
)

envoy_py_library(
    "tools.base.utils",
    deps = [
        requirement("pyyaml"),
        requirement("setuptools"),
    ],
)
