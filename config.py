def can_build(env, platform):
        return True

def configure(env):
        if env.msvc:
                env.Append(LINKFLAGS='liblua.lib')

def get_doc_classes():
    return [
        "Lua",
    ]

def get_doc_path():
    return "docs"