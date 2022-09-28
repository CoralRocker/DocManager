def Settings(**kwargs):
    return {'flags': 
            [ '-x', 'c++', '-std=c++20', '-Wall', '-Wextra', '-pedantic', '-Ilib/', '-Ilib/backends/']}
    if kwargs['language'] == 'c':
        return {'flags': ['-Wall', '-Wextra', 'Werror', '-std=c11']}
