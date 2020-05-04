load("//:.bazel_util.bzl", "config_setting_from_bool_flag")

zlog_optimize_options = {
    "light_weight_string": "-DLIGHT_WEIGHT_STRING_ENABLED",
}

def get_zlog_copts_all():
    return list(zlog_optimize_options.values())

def parse_zlog_copts_config():
    for option in zlog_optimize_options:
        config_setting_from_bool_flag(
            default_value = False,
            flag_name = option,
        )

def get_zlog_copts_config():
    zlog_copts_config = []
    for option in zlog_optimize_options:
        zlog_copts_config += select({
            "//conditions:default": [],
            "//zlog:" + option + "_enabled": [zlog_optimize_options[option]],
        })
    return zlog_copts_config
