from docutils import nodes
from docutils.parsers.rst import directives
from sphinx.util.docutils import SphinxDirective
from sphinx.directives.code import LiteralIncludeReader


def get_lines(filename, tag):
    # find "/**": start of doxygen comment
    #   and tag: name of function
    # l0: line number of "/**"
    # l1: line number of the end of declaration
    l0 = -1
    l1 = -1
    with open(filename, "r") as f:
        flag = False
        for cnt, line in enumerate(f):
            if "/**" in line:
                # doxygen comment found
                l0 = cnt
            if f" {tag}(" in line:
                # function found
                flag = True
            if flag and ");" in line:
                # function definition ends here
                l1 = cnt
                break
        if not flag:
            msg = f"declaration not found: {tag}"
            raise RuntimeError(msg)
    # index in python starts from 0, while line number is from 1
    l0 += 1
    l1 += 1
    if l0 <= 0:
        msg = f"non-positive l0: {l0}"
        raise RuntimeError(msg)
    if l1 <= 0:
        msg = f"non-positive l1: {l1}"
        raise RuntimeError(msg)
    if l1 <= l0:
        msg = f"l1 ({l1}) < l0 {l0}"
        raise RuntimeError(msg)
    return "{}-{}".format(l0, l1)


class MyDeclare(SphinxDirective):
    has_content = False
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {
        "language": directives.unchanged_required,
        "tag": directives.unchanged_required,
    }

    def run(self):
        document = self.state.document
        if not document.settings.file_insertion_enabled:
            return [document.reporter.warning("File insertion disabled",
                                              line=self.lineno)]
        try:
            rel_filename, filename = self.env.relfn2path(self.arguments[0])
            self.env.note_dependency(rel_filename)
            self.options["lines"] = get_lines(filename, self.options["tag"])
            reader = LiteralIncludeReader(filename, self.options, self.config)
            text, lines = reader.read()
            retnode = nodes.literal_block(text, text, source=filename)
            retnode["force"] = "force" in self.options
            self.set_source_info(retnode)
            retnode["language"] = self.options["language"]
            retnode["linenos"] = False
            self.add_name(retnode)
            return [retnode]
        except Exception as exc:
            return [document.reporter.warning(exc, line=self.lineno)]


def setup(app):
    app.add_directive("mydeclare", MyDeclare)
    return {
        "version": "0.1",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
