project(
	'wayfire',
	'c',
	'cpp',
	version: '0.4.0',
	license: 'MIT',
	meson_version: '>=0.47.0',
	default_options: [
		'cpp_std=c++17',
		'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

so_version = ['0', '4', '0']
wayfire_api_inc  = include_directories('src/api')

wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')
wayland_cursor = dependency('wayland-cursor')
wayland_protos = dependency('wayland-protocols', version: '>=1.12')
cairo          = dependency('cairo')
drm            = dependency('libdrm')
egl            = dependency('egl')
glesv2         = dependency('glesv2')
glm            = dependency('glm')
libinput       = dependency('libinput', version: '>=1.7.0')
pixman         = dependency('pixman-1')
threads        = dependency('threads')
xkbcommon      = dependency('xkbcommon')
wlroots        = dependency('wlroots', version: ['>=0.9.0', '<0.11.0'], required: get_option('use_system_wlroots'))
wfconfig       = dependency('wf-config', version: ['>=0.4.0', '<0.5.0'], required: get_option('use_system_wfconfig'))

use_system_wlroots = not get_option('use_system_wlroots').disabled() and wlroots.found()
if not use_system_wlroots
  wlroots = subproject('wlroots', default_options : ['examples=false']).get_variable('wlroots')
endif

use_system_wfconfig = not get_option('use_system_wfconfig').disabled() and wfconfig.found()
if not use_system_wfconfig
  wfconfig = subproject('wf-config').get_variable('wfconfig')
endif

needs_libinotify = ['freebsd', 'dragonfly'].contains(host_machine.system())
libinotify       = dependency('libinotify', required: needs_libinotify)

jpeg = dependency('libjpeg', required: false)
png  = dependency('libpng',  required: false)

# backtrace() is in a separate library on FreeBSD
backtrace = meson.get_compiler('cpp').find_library('execinfo', required: false)

conf_data = configuration_data()

conf_data.set('WF_SRC_DIR', meson.source_root())
conf_data.set('INSTALL_PREFIX', get_option('prefix'))
conf_data.set('PLUGIN_PATH', join_paths(get_option('prefix'), get_option('libdir'), 'wayfire'))
metadata_dir_suffix = 'share/wayfire/metadata'
conf_data.set('PLUGIN_XML_DIR', join_paths(get_option('prefix'), metadata_dir_suffix))
sysconfdir = join_paths(get_option('prefix'), get_option('sysconfdir'))
conf_data.set('SYSCONFDIR', sysconfdir)

# needed to dlopen() plugins
# -E is for RTTI/dynamic_cast across plugins
add_project_arguments(['-DWLR_USE_UNSTABLE'], language: ['cpp', 'c'])
add_project_link_arguments(['-rdynamic', '-Wl,-E'], language: 'cpp')

if get_option('enable_gles32') and meson.get_compiler('cpp').has_header(
    'GLES3/gl32.h', args: '-I' + glesv2.get_pkgconfig_variable('includedir'))
  conf_data.set('USE_GLES32', true)
else
  conf_data.set('USE_GLES32', false)
endif

if png.found() and jpeg.found()
  conf_data.set('BUILD_WITH_IMAGEIO', true)
else
  conf_data.set('BUILD_WITH_IMAGEIO', false)
endif

configure_file(input: 'config.h.in',
               output: 'config.h',
               configuration: conf_data)

wayfire_conf_inc = include_directories(['.'])

add_project_arguments(['-Wno-unused-parameter'], language: 'cpp')

subdir('proto')
subdir('src')
subdir('metadata')
subdir('plugins')

wlroots_x_info = []

if use_system_wlroots
  cpp = meson.get_compiler('cpp')
  wlroots_has_xwayland = cpp.get_define('WLR_HAS_XWAYLAND', prefix: '#include <wlr/config.h>', dependencies: wlroots) == '1'
  wlroots_has_x11_backend = cpp.get_define('WLR_HAS_X11_BACKEND', prefix: '#include <wlr/config.h>', dependencies: wlroots) == '1'
  wlroots_x_info += [
      '       xwayland: @0@'.format(wlroots_has_xwayland),
      '    x11-backend: @0@'.format(wlroots_has_x11_backend)]
endif

summary_begin = [
	'',
	'----------------',
	'wayfire @0@'.format(meson.project_version()),
	'',
    'system wfconfig: @0@'.format(use_system_wfconfig),
    ' system wlroots: @0@'.format(use_system_wlroots)]

summary_end = [
    '        imageio: @0@'.format(conf_data.get('BUILD_WITH_IMAGEIO')),
    '         gles32: @0@'.format(conf_data.get('USE_GLES32')),
    '----------------',
    ''
]
message('\n'.join(summary_begin + wlroots_x_info + summary_end))
