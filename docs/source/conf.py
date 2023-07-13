import os
import sys

paths = ("./ext", )
for path in paths:
    sys.path.append(os.path.abspath(path))

project = "SimpleNpyIO"
author = "Naoki Hori"
copyright = f"2022, {author}"

extensions = [
        "mydeclare",
        "sphinx.ext.graphviz",
]

html_theme = "alabaster"
html_theme_options = {
        "description": "C99-compatible NPY file reader / writer",
        "fixed_sidebar": "false",
        "github_banner": "false",
        "github_button": "true",
        "github_count": "true",
        "github_repo": "SimpleNpyIO",
        "github_type": "star",
        "github_user": "NaokiHori",
        "navigation_with_keys": "true",
        "nosidebar": "false",
        "show_powered_by": "true",
        "show_related": "false",
        "show_relbars": "false",
        "sidebar_collapse": "true",
        "sidebar_includehidden": "false",
        "page_width": "95vw",
        "sidebar_width": "20vw",
}

graphviz_output_format = "svg"

