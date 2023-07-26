add_rules('mode.debug', 'mode.release')
set_symbols('debug')

target('libmqttsn')
    set_kind('static')
    
    add_files('*.cpp')


target('snc')
    set_kind('binary')
    
    add_deps('libmqttsn')
    add_files('test/client.cpp')