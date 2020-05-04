""" some bazel util macros """

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag", "int_flag", "string_flag")

def config_setting_from_bool_flag(flag_name, default_value):
    bool_flag(
        name = flag_name,
        build_setting_default = default_value and 1 or 0,
    )
    native.config_setting(
        name = flag_name + "_enabled",
        flag_values = {flag_name: "1"},
    )

def config_settings_from_int_flag(flag_name, default_value, config):
    int_flag(
        name = flag_name,
        build_setting_default = default_value,
    )
    for config_value in config:
        native.config_setting(
            name = flag_name + "_" + str(config_value),
            flag_values = {flag_name: str(config_value)},
        )

def config_settings_from_string_flag(flag_name, default_value, config):
    string_flag(
        name = flag_name,
        build_setting_default = default_value,
    )
    for config_value in config:
        native.config_setting(
            name = flag_name + "_" + config_value,
            flag_values = {flag_name: config_value},
        )
