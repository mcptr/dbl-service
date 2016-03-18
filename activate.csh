# This file must be used with "source bin/activate.csh" *from csh*.
# You cannot run it directly.
# Created by Davide Di Blasi <davidedb@gmail.com>.
# Ported to Python 3.3 venv by Andrew Svetlov <andrew.svetlov@gmail.com>

alias deactivate 'test $?_OLD_VIRTUAL_PATH != 0 && setenv PATH "$_OLD_VIRTUAL_PATH" && unset _OLD_VIRTUAL_PATH; rehash; test $?_OLD_VIRTUAL_PROMPT != 0 && set prompt="$_OLD_VIRTUAL_PROMPT" && unset _OLD_VIRTUAL_PROMPT; unsetenv VIRTUAL_ENV; test "\!:*" != "nondestructive" && unalias deactivate'

# Unset irrelavent variables.
deactivate nondestructive


setenv VIRTUAL_ENV "$HOME/Devel/dbl"
setenv PROJECT_ROOT "$VIRTUAL_ENV/project"
setenv LD_LIBRARY_PATH "$VIRTUAL_ENV/lib:$VIRTUAL_ENV/lib64:$VIRTUAL_ENV/project/service/lib"

set _OLD_VIRTUAL_PATH="$PATH"
setenv PATH "$VIRTUAL_ENV/bin:$PATH"
setenv PYTHONPATH "$PROJECT_ROOT/src"


set _OLD_VIRTUAL_PROMPT="$prompt"

if (! "$?VIRTUAL_ENV_DISABLE_PROMPT") then
    set env_name = "dbl"
    set prompt = "[$env_name] $prompt"
    unset env_name
endif

alias pydoc python -m pydoc

rehash
