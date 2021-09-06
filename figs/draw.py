from types import CodeType
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import math
import matplotlib.mlab as mlab
import matplotlib.ticker as ticker
from mpl_toolkits.axes_grid1 import host_subplot
import mpl_toolkits.axisartist as AA
import matplotlib.patches as mpatches
import matplotlib.lines as mlines
import json

import matplotlib
from numpy.lib.function_base import cov
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
dirbase = 'figs/'
ourSys = 'APE'

sz, fontsz = (6, 3), 16
figsz = {
    'axes.labelsize': fontsz,
    'font.size': fontsz,
    'legend.fontsize': fontsz,
    'xtick.labelsize': fontsz,
    'ytick.labelsize': fontsz,
    'figure.figsize': sz,
}
plt.rcParams.update(figsz)

hb = '\\\\//\\\\//'

color_def = [
    '#f4b183',
    '#ffd966',
    '#c5e0b4',
    '#bdd7ee',
    "#8dd3c7",
    "#bebada",
    "#fb8072",
    "#80b1d3",
    "#fdb462",
    "#cccccc",
    "#fccde5",
    "#b3de69",
    "#ffd92f",
    '#fc8d59',
    '#74a9cf',
    '#66c2a4',
    '#f4a143',
    '#ffc936',
    '#78c679',
]

color_line = [
    '#5AA469',
    '#1A508B',
    '#F39233',
    '#B61919',
]

hatch_def = [
    '//',
    '\\\\',
    'xx',
    '++',
    '--',
    '||',
    '..',
    'oo',
    '',
]

marker_def = [
    'o',
    'x',
    'D',
    '*',
    '+',
]


def test_color(color_vec=color_def, hatch_vec=hatch_def, marker_def=marker_def, path=''):
    figsz = {'figure.figsize': (8, 3)}
    plt.rcParams.update(figsz)
    N = len(color_vec)
    dat = [1 for i in range(N)]
    fig, ax = plt.subplots()
    W = 0.35
    ind = np.arange(N) - 2 * W
    ax.bar(ind, dat, color=color_vec)
    bars = ax.patches
    hatches = []
    for i in range(N):
        hatches.append(hatch_vec[i % len(hatch_vec)])
    for bar, hatch in zip(bars, hatches):
        bar.set_hatch(hatch)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], hatch=hatches[i], label=str(i)) for i in range(N)]
    plt.legend(legend_handles, range(
        N), bbox_to_anchor=(1, 1), ncol=2)
    plt.subplots_adjust(bottom=0.2)
    xvals = np.arange(N)
    ax.set_xticks(ind)
    ax.set_xticklabels(str(x) for x in xvals)
    plt.show()
    if path != '':
        fig.savefig(path, bbox_inches='tight')


# test_color(color_def, hatch_def)


def draw_malloc_size():
    dat_malloc = []
    dat_swalloc = []
    dat_free = []
    dat_swfree = []
    dat_size = []
    infile = 'memtest_log/size.log'
    state = ''
    with open(infile) as f:
        lines = f.readlines()
        for l in lines:
            if l.startswith('Testing malloc'):
                state = 'malloc'
            elif l.startswith('Testing BlockAllocator'):
                state = 'swalloc'
            elif l.startswith('Testing'):
                state = ''
            if l.startswith('Allocate size'):
                (size_str, alloc_str, free_str) = l.split(',')
                alloc_size = int(size_str.split()[-1])
                alloc_time = float(alloc_str.split()[-1][:-2])
                free_time = float(free_str.split()[-1][:-3])
                if state == 'malloc':
                    dat_size.append(alloc_size)
                    dat_malloc.append(alloc_time)
                    dat_free.append(free_time)
                if state == 'swalloc':
                    dat_swalloc.append(alloc_time)
                    dat_swfree.append(free_time)
    # print(dat_size)
    # print(dat_malloc)
    # print(dat_free)
    # print(dat_swalloc)
    # print(dat_swfree)
    dat = [dat_malloc, dat_free, dat_swalloc, dat_swfree]
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 5), 10
    figsz = {
        'axes.labelsize': fontsz,
        'font.size': fontsz,
        'legend.fontsize': fontsz,
        'xtick.labelsize': fontsz,
        'ytick.labelsize': fontsz,
        'figure.figsize': sz,
    }
    figsz = {'figure.figsize': sz}
    
    plt.rcParams.update(figsz)

    fig, ax = plt.subplots()
    color_vec = color_def
    hatch_vec = [None] * num_bars

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar((width * num_types + ggap) * i + width * j + (sum(lgap[:j])),
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_yscale('log')
    ax.set_xticks([(width * num_types + ggap) * i + (width*3/2 + lgap[1]/2)
                   for i in range(num_bars)])
    ax.set_xticklabels(['2^'+str(int(math.log(s,2))) for s in dat_size], rotation=60, fontsize=12)
    ax.set_ylabel("Time (ms)")
    # ax.set_ylim(0, 300)
    types = ['malloc', 'free', 'swalloc', 'swfree']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles)

    plt.show()
    fig.savefig(dirbase + 'malloc_size.pdf', bbox_inches='tight')

# draw_malloc_size()

def draw_malloc():
    dat_alloc_x86 = [0.000976562, 0, 0, 0, 0, 0, 0, 0, 0.00805664, 0, 0, 0.00390625, 0.00488281, 0.00390625, 0.00512695, 0.0090332, 0.00610352, 0.00610352, 0.00488281, 0.0090332, 0.0090332, 0.00805664, 0.0078125, 0.00708008, 0.00805664, 0.0078125]
    dat_alloc_sw = [0.00390625, 0.000976562, 0.000976562, 0.00195312, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0.00292969, 0.000976562, 0.00195312, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0.748047, 1.3728, 2.68115, 5.30298, 10.5801, 21.3091, 42.438, 84.4351]

    # dat_free_x86 = [0.00195312, 0, 0, 0, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0, 0, 0, 0, 0, 0, 0, 0.0109863, 0.00585938, 0.00390625, 0.00415039, 0.0100098, 0.0129395, 0.00708008, 0.00708008, 0.00683594, 0.00708008, 0.00805664]
    # dat_free_sw = [0.0012207, 0.0012207, 0.000976562, 0, 0, 0.00195312, 0.000976562, 0, 0.000976562, 0.00195312, 0.000976562, 0.000976562, 0.000976562, 0, 0, 0, 0.000976562, 0, 0.0939941, 0.136963, 0.276855, 0.454102, 0.878174, 1.78198, 4.46094, 8.78101]
    dat_size = [4 * 2 ** x for x in range(26)]
    # dat = [dat_alloc_x86, dat_alloc_sw, dat_free_x86, dat_free_sw]
    dat = [dat_alloc_x86, dat_alloc_sw]
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 5), 10
    figsz = {
        'axes.labelsize': fontsz,
        'font.size': fontsz,
        'legend.fontsize': fontsz,
        'xtick.labelsize': fontsz,
        'ytick.labelsize': fontsz,
        'figure.figsize': sz,
    }
    figsz = {'figure.figsize': sz}
    
    plt.rcParams.update(figsz)

    fig, ax = plt.subplots()
    color_vec = color_def
    hatch_vec = [None] * num_bars

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar((width * num_types + ggap) * i + width * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_xticks([(width * num_types + ggap) * i + width*num_types/2 for i in range(num_bars)])
    ax.set_xticklabels([str(2**x)+"B" for x in range(2, 10)] + [str(2**x)+"KB" for x in range(10)] + [str(2**x)+"MB" for x in range(8)], rotation=60, fontsize=12)
    ax.set_xlim(0, (width * num_types + ggap) * num_bars - ggap)
    ax.set_yscale('log')
    ax.set_ylabel("Time (ms)")
    ax.set_ylim(0, 100)
    # ax.set_yticks([0.01, 0.1, 1, 10, 100])
    types = ['x86', 'sw']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles)

    # fig.show()
    fig.savefig(dirbase + 'malloc.pdf', bbox_inches='tight')

# draw_malloc()

def draw_malloc_random_test():
    dat_malloc = []
    dat_falloc_first = []
    dat_falloc_best = []
    dat_swalloc = []
    dat_swalloc_single = []
    dat = [dat_malloc, dat_falloc_first, dat_falloc_best, dat_swalloc, dat_swalloc_single]
    dat_size = ['1M', '2M', '4M', '8M']
    dat_num = [256, 512, 1024, 2048, 4096, 8192, 16384]
    sz = dat_size[3]
    for type in range(5):
        for num in dat_num:
            filename = 'memtest_log/rand_%s_%d.%d.log' % (sz, num, type)
            with open(filename) as fin:
                lines = fin.readlines()
                assert(lines[1].startswith('Time init'))
                assert(lines[2].startswith('Time trace'))
                t_init = float(lines[1].split(':')[1].strip().split()[0])
                t_trace = float(lines[2].split(':')[1].strip().split()[0])
                t_total = t_init + t_trace
                dat[type].append(t_total)
    # print(dat)

    dat = [dat[0], dat[2], dat[3], dat[4]]
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 5), 10
    figsz = {
        'axes.labelsize': fontsz,
        'font.size': fontsz,
        'legend.fontsize': fontsz,
        'xtick.labelsize': fontsz,
        'ytick.labelsize': fontsz,
        'figure.figsize': sz,
    }
    figsz = {'figure.figsize': sz}
    
    plt.rcParams.update(figsz)

    fig, ax = plt.subplots()
    color_vec = color_def
    hatch_vec = [None] * num_bars

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar((width * num_types + ggap) * i + width * j + (sum(lgap[:j])),
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_yscale('log')
    ax.set_xticks([(width * num_types + ggap) * i + (width*3/2 + lgap[1]/2)
                   for i in range(num_bars)])
    ax.set_xticklabels(['2^'+str(int(math.log(s,2))) for s in dat_num], rotation=60, fontsize=12)
    ax.set_ylabel("Time (ms)")
    # ax.set_ylim(0, 300)
    types = ['malloc', 'freelist', 'swalloc', 'swalloc_single']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles)

    plt.show()
    fig.savefig(dirbase + 'random_test.pdf', bbox_inches='tight')


# draw_malloc_random_test()

def draw_malloc_parsec_test():
    dat_malloc = []
    dat_falloc_first = []
    dat_falloc_best = []
    dat_swalloc = []
    dat_swalloc_single = []
    dat = [dat_malloc, dat_falloc_first, dat_falloc_best, dat_swalloc, dat_swalloc_single]
    dat_prog = ['blackscholes', 'bodytrack', 'canneal', 'dedup', 'facesim', 'ferret', 'fluidanimate', 'freqmine', 'raytrace', 'streamcluster', 'swaptions', 'vips', 'x264']
    for type in range(5):
        for prog in dat_prog:
            filename = 'memtest_log/%s.%d.log' % (prog, type)
            with open(filename) as fin:
                lines = fin.readlines()
                if len(lines) < 3:
                    dat[type].append(0)
                    continue
                assert(lines[1].startswith('Time init'))
                assert(lines[2].startswith('Time trace'))
                t_init = float(lines[1].split(':')[1].strip().split()[0])
                t_trace = float(lines[2].split(':')[1].strip().split()[0])
                t_total = t_init + t_trace
                dat[type].append(t_total)
    # print(dat)

    dat = [dat[0], dat[2], dat[3], dat[4]]
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 5), 10
    figsz = {
        'axes.labelsize': fontsz,
        'font.size': fontsz,
        'legend.fontsize': fontsz,
        'xtick.labelsize': fontsz,
        'ytick.labelsize': fontsz,
        'figure.figsize': sz,
    }
    figsz = {'figure.figsize': sz}
    
    plt.rcParams.update(figsz)

    fig, ax = plt.subplots()
    color_vec = color_def
    hatch_vec = [None] * num_bars

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar((width * num_types + ggap) * i + width * j + (sum(lgap[:j])),
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_yscale('log')
    ax.set_xticks([(width * num_types + ggap) * i + (width*3/2 + lgap[1]/2)
                   for i in range(num_bars)])
    ax.set_xticklabels([prog for prog in dat_prog], rotation=60, fontsize=12)
    ax.set_ylabel("Time (ms)")
    # ax.set_ylim(0, 300)
    types = ['malloc', 'freelist', 'swalloc', 'swalloc_single']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles)

    plt.show()
    fig.savefig(dirbase + 'parsec_test.pdf', bbox_inches='tight')


# draw_malloc_parsec_test()


def draw_malloc_bgl_test():
    dat_malloc = []
    dat_falloc_first = []
    dat_falloc_best = []
    dat_swalloc = []
    dat_swalloc_single = []
    dat = [dat_malloc, dat_falloc_first, dat_falloc_best, dat_swalloc, dat_swalloc_single]
    dat_size = [3072, 6144]
    for type in range(5):
        for size in dat_size:
            filename = 'memtest_log/bgl_%s.%d.log' % (size, type)
            with open(filename) as fin:
                lines = fin.readlines()
                if len(lines) < 3:
                    dat[type].append(0)
                    continue
                assert(lines[1].startswith('Time init'))
                assert(lines[2].startswith('Time trace'))
                t_init = float(lines[1].split(':')[1].strip().split()[0])
                t_trace = float(lines[2].split(':')[1].strip().split()[0])
                t_total = t_init + t_trace
                dat[type].append(t_total)
    # print(dat)

    dat = [dat[0], dat[2], dat[3], dat[4]]
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 5), 10
    figsz = {
        'axes.labelsize': fontsz,
        'font.size': fontsz,
        'legend.fontsize': fontsz,
        'xtick.labelsize': fontsz,
        'ytick.labelsize': fontsz,
        'figure.figsize': sz,
    }
    figsz = {'figure.figsize': sz}
    
    plt.rcParams.update(figsz)

    fig, ax = plt.subplots()
    color_vec = color_def
    hatch_vec = [None] * num_types

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar((width * num_types + ggap) * i + width * j + (sum(lgap[:j])),
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_yscale('log')
    ax.set_xticks([(width * num_types + ggap) * i + (width*3/2 + lgap[1]/2)
                   for i in range(num_bars)])
    ax.set_xticklabels([s for s in dat_size], rotation=60, fontsize=12)
    ax.set_ylabel("Time (ms)")
    # ax.set_ylim(0, 300)
    types = ['malloc', 'freelist', 'swalloc', 'swalloc_single']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles)

    plt.show()
    fig.savefig(dirbase + 'bgl_test.pdf', bbox_inches='tight')


draw_malloc_bgl_test()
