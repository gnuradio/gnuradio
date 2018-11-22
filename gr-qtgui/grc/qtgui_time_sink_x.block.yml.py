#!/usr/bin/env python

import math
import re

EVERYTHING_BEFORE_PARAMS = """id: qtgui_time_sink_x
label: QT GUI Time Sink

parameters:
-   id: type
    label: Type
    dtype: enum
    default: complex
    options: [complex, float, msg_complex, msg_float]
    option_labels: [Complex, Float, Complex Message, Float Message]
    option_attributes:
        fcn: [time_sink_c, time_sink_f, time_sink_c, time_sink_f]
        t: [complex, float, message, message]
    hide: part
-   id: name
    label: Name
    dtype: string
    default: '""'
    hide: ${ ('none' if len(name) > 0 else 'part') }
-   id: ylabel
    label: Y Axis Label
    dtype: string
    default: Amplitude
    hide: part
-   id: yunit
    label: Y Axis Unit
    dtype: string
    default: '""'
    hide: part
-   id: size
    label: Number of Points
    dtype: int
    default: '1024'
    hide: ${ ('all' if type.startswith('msg') else 'none') }
-   id: srate
    label: Sample Rate
    dtype: float
    default: samp_rate
-   id: grid
    label: Grid
    dtype: enum
    default: 'False'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: part
-   id: autoscale
    label: Autoscale
    dtype: enum
    default: 'False'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
-   id: ymin
    label: Y min
    dtype: float
    default: '-1'
    hide: part
-   id: ymax
    label: Y max
    dtype: float
    default: '1'
    hide: part
-   id: nconnections
    label: Number of Inputs
    dtype: int
    default: '1'
    hide: ${ ('all' if type.startswith('msg') else 'part') }
-   id: update_time
    label: Update Period
    dtype: float
    default: '0.10'
    hide: part
-   id: entags
    label: Disp. Tags
    dtype: enum
    default: 'True'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: ${ ('all' if type.startswith('msg') else 'part') }
-   id: gui_hint
    label: GUI Hint
    dtype: gui_hint
    hide: part
-   id: tr_mode
    label: Trigger Mode
    category: Trigger
    dtype: enum
    default: qtgui.TRIG_MODE_FREE
    options: [qtgui.TRIG_MODE_FREE, qtgui.TRIG_MODE_AUTO, qtgui.TRIG_MODE_NORM, qtgui.TRIG_MODE_TAG]
    option_labels: [Free, Auto, Normal, Tag]
    hide: part
-   id: tr_slope
    label: Trigger Slope
    category: Trigger
    dtype: enum
    default: qtgui.TRIG_MODE_POS
    options: [qtgui.TRIG_SLOPE_POS, qtgui.TRIG_SLOPE_NEG]
    option_labels: [Positive, Negative]
    hide: part
-   id: tr_level
    label: Trigger Level
    category: Trigger
    dtype: float
    default: '0.0'
    hide: part
-   id: tr_delay
    label: Trigger Delay
    category: Trigger
    dtype: float
    default: '0'
    hide: part
-   id: tr_chan
    label: Trigger Channel
    category: Trigger
    dtype: int
    default: '0'
    hide: part
-   id: tr_tag
    label: Trigger Tag Key
    category: Trigger
    dtype: string
    default: '""'
    hide: part
-   id: ctrlpanel
    label: Control Panel
    category: Config
    dtype: enum
    default: 'False'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: part
-   id: legend
    label: Legend
    category: Config
    dtype: enum
    default: 'True'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: part
-   id: axislabels
    label: Axis Labels
    category: Config
    dtype: enum
    default: 'True'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: part
-   id: stemplot
    label: Stem Plot
    category: Config
    dtype: enum
    default: 'False'
    options: ['True', 'False']
    option_labels: ['Yes', 'No']
    hide: part"""

LINE_PARAMS = """
-   id: label{i}
    label: Line {i} Label
    dtype: string
    default: 'Signal {i}'
    base_key: label1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
-   id: width{i}
    label: Line {i} Width
    default: 1
    base_key: width1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
-   id: color{i}
    label: Line {i} Color
    dtype: string
    default: '{i_color}'
    base_key: color1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
-   id: style{i}
    label: Line {i} Style
    dtype: int
    default: 1
    base_key: style1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
-   id: marker{i}
    label: Line {i} Marker
    dtype: int
    default: 1
    base_key: marker1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
-   id: alpha{i}
    label: Line {i} Alpha
    dtype: real
    default: 1.0
    base_key: alpha1
    hide: ${{ ('part' if (
            int(nconnections) >= {i}
            or (type == "complex" and int(nconnections) >= {i_cplx})
            or (type == "msg_complex")) and (not type == "msg_float")
        else 'all')
        }}
"""

EVERYTHING_AFTER_PARAMS = """
asserts:
- ${nconnections <= (5 if type == 'complex' else 10)}

inputs:
-   domain: stream
    dtype: ${ type.t }
    multiplicity: ${ (0 if type.startswith('msg') else nconnections) }
    optional: true

templates:
    imports: |-
        from PyQt5 import Qt
        from gnuradio import qtgui
        from gnuradio.filter import firdes
        import sip
    callbacks:
    - set_time_domain_axis(${min}, ${max})
    - set_update_time(${update_time})
    - set_y_axis(${ymin}, ${ymax})
    - set_samp_rate(${srate})
    - self.${id}.set_trigger_mode(${tr_mode}, ${tr_slope}, ${tr_level}, ${tr_delay},
        ${tr_chan}, ${tr_tag})
    make: |-
        <%
            win = 'self._%s_win'%id
        %>\\
        qtgui.${type.fcn}(
            ${size}, #size
            ${srate}, #samp_rate
            ${name}, #name
            ${0 if type.startswith('msg') else nconnections} #number of inputs
        )
        self.${id}.set_update_time(${update_time})
        self.${id}.set_y_axis(${ymin}, ${ymax})

        self.${id}.set_y_label(${ylabel}, ${yunit})

        self.${id}.enable_tags(${entags})
        self.${id}.set_trigger_mode(${tr_mode}, ${tr_slope}, ${tr_level}, ${tr_delay}, ${tr_chan}, ${tr_tag})
        self.${id}.enable_autoscale(${autoscale})
        self.${id}.enable_grid(${grid})
        self.${id}.enable_axis_labels(${axislabels})
        self.${id}.enable_control_panel(${ctrlpanel})
        self.${id}.enable_stem_plot(${stemplot})

        % if legend == "False":
        self.${id}.disable_legend()
        % endif

        labels = [${label1}, ${label2}, ${label3}, ${label4}, ${label5},
            ${label6}, ${label7}, ${label8}, ${label9}, ${label10}]
        widths = [${width1}, ${width2}, ${width3}, ${width4}, ${width5},
            ${width6}, ${width7}, ${width8}, ${width9}, ${width10}]
        colors = [${color1}, ${color2}, ${color3}, ${color4}, ${color5},
            ${color6}, ${color7}, ${color8}, ${color9}, ${color10}]
        alphas = [${alpha1}, ${alpha2}, ${alpha3}, ${alpha4}, ${alpha5},
            ${alpha6}, ${alpha7}, ${alpha8}, ${alpha9}, ${alpha10}]
        styles = [${style1}, ${style2}, ${style3}, ${style4}, ${style5},
            ${style6}, ${style7}, ${style8}, ${style9}, ${style10}]
        markers = [${marker1}, ${marker2}, ${marker3}, ${marker4}, ${marker5},
            ${marker6}, ${marker7}, ${marker8}, ${marker9}, ${marker10}]


        % if type.endswith('complex'):
        for i in range(${2 if type.startswith('msg') else 2*int(nconnections)}):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
                    self.${id}.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.${id}.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.${id}.set_line_label(i, labels[i])
            self.${id}.set_line_width(i, widths[i])
            self.${id}.set_line_color(i, colors[i])
            self.${id}.set_line_style(i, styles[i])
            self.${id}.set_line_marker(i, markers[i])
            self.${id}.set_line_alpha(i, alphas[i])
        % else:
        for i in range(${1 if type.startswith('msg') else int(nconnections)}):
            if len(labels[i]) == 0:
                self.${id}.set_line_label(i, "Data {0}".format(i))
            else:
                self.${id}.set_line_label(i, labels[i])
            self.${id}.set_line_width(i, widths[i])
            self.${id}.set_line_color(i, colors[i])
            self.${id}.set_line_style(i, styles[i])
            self.${id}.set_line_marker(i, markers[i])
            self.${id}.set_line_alpha(i, alphas[i])
        % endif

        ${win} = sip.wrapinstance(self.${id}.pyqwidget(), Qt.QWidget)
        ${gui_hint() % win}

documentation: |-
    The GUI hint can be used to position the widget within the application. The hint is of the form [tab_id@tab_index]: [row, col, row_span, col_span]. Both the tab specification and the grid position are optional.

file_format: 1
"""

def make_yml():
    """Return the YML file as a string"""
    default_colors = [
        'blue', 'red', 'green', 'black', 'cyan', 'magenta', 'yellow',
        'dark red', 'dark green', 'Dark Blue'
    ]
    line_params_1 = LINE_PARAMS.format(i=1, i_cplx=1, i_color=default_colors[0])
    line_params_1 = re.sub(r'    base_key:.*\n', '', line_params_1)
    line_params_n = ''.join([
        LINE_PARAMS.format(
            i=i,
            i_cplx=int(math.ceil(float(i)/2)),
            i_color=default_colors[i % len(default_colors)],
        )
        for i in range(2, 11)
    ])
    return ''.join((
        EVERYTHING_BEFORE_PARAMS,
        line_params_1,
        line_params_n,
        EVERYTHING_AFTER_PARAMS,
    ))


if __name__ == '__main__':
    import sys
    try:
        filename = sys.argv[1]
    except IndexError:
        filename = __file__[:-3]
    data = make_yml()
    with open(filename, 'wb') as fp:
        fp.write(data.encode())

