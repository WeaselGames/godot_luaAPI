def can_build(env, platform):
        return True

def configure(env):
        if env.msvc:
                env.Append(LINKFLAGS='liblua.lib')