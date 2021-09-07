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
matplotlib.rcParams['font.sans-serif'] = ['Microsoft YaHei'] # YaHei is one common Chinese font
matplotlib.rcParams['axes.unicode_minus'] = False # Repair the bug of representing '-'as "square"

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
    # 'font.family': 'SimHei',
}
plt.rcParams.update(figsz)

hb = '\\\\//\\\\//'

color_def = [
    '#ffd966',
    '#c5e0b4',
    '#f4b183',
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


def draw_malloc_size(ty):
    dat_malloc = []
    dat_swalloc = []
    dat_free = []
    dat_swfree = []
    dat_size = []

    dat_malloc = [0.000476837, 0.00066185, 0.000469208, 0.000455856, 0.000471115, 0.000577927, 0.000600815, 0.000583649, 0.000591278, 0.000413895, 0.000480652, 0.000473022, 0.000465393, 0.000488281, 0.000486374, 0.000465393, 0.000453949, 0.000488281, 0.000505447, 0.000509262, 0.000482559, 0.000492096, 0.000484467, 21.2865, 42.4472, 84.8605]
    dat_free = [0.000434875, 0.000465393, 0.000436783, 0.000406265, 0.000431061, 0.000450134, 0.000423431, 0.000440598, 0.000404358, 0.000452042, 0.000476837, 0.000490189, 0.000476837, 0.000490189, 0.000482559, 0.000484467, 0.000476837, 0.000494003, 0.000427246, 0.000406265, 0.000429153, 0.000427246, 0.00044632, 1.72853, 4.48492, 8.79831]
    dat_swalloc = [0.000602722, 0.000621796, 0.000600815, 0.000608444, 0.000579834, 0.000768661, 0.000745773, 0.000713348, 0.000692368, 0.00060463, 0.00060463, 0.000635147, 0.00062561, 0.000669479, 0.000680923, 0.000598907,  0.000930786, 0.000905991, 0.000905991, 0.00091362, 0.00115967, 0.000959396, 0.000930786, 0.00107002, 0.000999451, 0.000919342]
    dat_swfree = [0.000535965, 0.000600815, 0.000576019, 0.000637054, 0.000600815, 0.000566483, 0.000593185, 0.000555038, 0.00053978, 0.00053215, 0.000585556, 0.000553131, 0.000528336,  0.000549316, 0.000545502,  0.000537872, 0.00135994, 0.001297, 0.0012455, 0.00153351, 0.00132942, 0.00122643, 0.0015049, 0.00132179, 0.00145149, 0.00153923]
    dat = [dat_malloc, dat_swalloc] if ty == 0 else [dat_free, dat_swfree]

    dat = np.log10(dat) + 4
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 4), 10
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
    hatch_vec = [None] + [hatch_def[0]]

    width, ggap, lgap = 0.35, 0.35, [0, 0.1, 0]
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar(ggap + (width * num_types + ggap) * i + width * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_xticks([ggap + (width * num_types + ggap) * i + width*num_types/2 for i in range(num_bars)])
    ax.set_xticklabels([str(2**x)+"B" for x in range(2, 10)] + [str(2**x)+"KB" for x in range(10)] + [str(2**x)+"MB" for x in range(8)], rotation=60, fontsize=12)
    ax.set_xlim(0, (width * num_types + ggap) * num_bars + ggap)
    ax.set_xlabel("分配长度")
    # ax.set_yscale('log')
    ax.set_ylabel("时间 (ms)")
    ax.set_ylim(0, 7)
    ax.set_yticks(range(8))
    ax.set_yticklabels(["$10^{" + str(x-4) + "}$" for x in range(8)])
    if ty == 0:
        types = ['malloc', 'SWAlloc-alloc']
    else:
        types = ['free', 'SWAlloc-free']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles, ncol=2, loc="upper center") #, bbox_to_anchor=(0.5, 1.2))


    # plt.show()
    fig.savefig(dirbase + ('malloc' if ty == 0 else 'free') + '_size.pdf', bbox_inches='tight')

draw_malloc_size(0)
draw_malloc_size(1)

def draw_arch_malloc():
    dat_alloc_x86 = [0.0000915527, 0.000087738, 0.0000915527, 0.0000858307, 0.0000934601, 0.000116348, 0.000123978, 0.000101089, 0.0000915527, 0.0000839233, 0.000116348, 0.0000934601, 0.000110626, 0.0000915527, 0.000108719, 0.0000858307, 0.000106812, 0.000106812, 0.000106812, 0.000101089, 0.0000991821, 0.000110626, 0.000114441, 0.00747299, 0.0074234, 0.00723839]
    dat_alloc_sw = [0.000476837, 0.00066185, 0.000469208, 0.000455856, 0.000471115, 0.000577927, 0.000600815, 0.000583649, 0.000591278, 0.000413895, 0.000480652, 0.000473022, 0.000465393, 0.000488281, 0.000486374, 0.000465393, 0.000453949, 0.000488281, 0.000505447, 0.000509262, 0.000482559, 0.000492096, 0.000484467, 21.2865, 42.4472, 84.8605]

    # dat_free_x86 = [0.00195312, 0, 0, 0, 0.000976562, 0.000976562, 0.000976562, 0.000976562, 0, 0, 0, 0, 0, 0, 0, 0.0109863, 0.00585938, 0.00390625, 0.00415039, 0.0100098, 0.0129395, 0.00708008, 0.00708008, 0.00683594, 0.00708008, 0.00805664]
    # dat_free_sw = [0.0012207, 0.0012207, 0.000976562, 0, 0, 0.00195312, 0.000976562, 0, 0.000976562, 0.00195312, 0.000976562, 0.000976562, 0.000976562, 0, 0, 0, 0.000976562, 0, 0.0939941, 0.136963, 0.276855, 0.454102, 0.878174, 1.78198, 4.46094, 8.78101]
    dat_size = [4 * 2 ** x for x in range(26)]
    # dat = [dat_alloc_x86, dat_alloc_sw, dat_free_x86, dat_free_sw]
    dat = [dat_alloc_x86, dat_alloc_sw]

    dat = np.log10(dat) + 5
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 4), 10
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
            ax.bar(ggap + (width * num_types + ggap) * i + width * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    ax.set_xticks([ggap + (width * num_types + ggap) * i + width*num_types/2 for i in range(num_bars)])
    ax.set_xticklabels([str(2**x)+"B" for x in range(2, 10)] + [str(2**x)+"KB" for x in range(10)] + [str(2**x)+"MB" for x in range(8)], rotation=60, fontsize=12)
    ax.set_xlim(0, (width * num_types + ggap) * num_bars + ggap)
    # ax.set_yscale('log')
    ax.set_ylabel("时间 (ms)")
    ax.set_ylim(0, 8)
    ax.set_yticks(range(9))
    ax.set_yticklabels(["$10^{" + str(x-5) + "}$" for x in range(9)])
    types = ['x86', 'sw']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles, ncol=2, loc="upper center") #, bbox_to_anchor=(0.5, 1.2))

    # fig.show()
    fig.savefig(dirbase + 'arch_malloc.pdf', bbox_inches='tight')

draw_arch_malloc()

def draw_malloc_random_test():
    dat_malloc = [2353.0, 3394.22, 5433.57, 9767.84, 18965.2, 33735.9, 61336.9]
    dat_falloc_first = [3541.04, 4605.74, 11746.2, 27433.7, 62224.1, 121684.0, 206989.0]
    dat_swalloc_single = [580.243, 590.392, 626.943, 659.851, 678.735, 677.906, 631.298]
    dat_swalloc = [574.702, 587.067, 620.957, 658.227, 680.242, 687.606, 651.064]

    dat = [dat_malloc, dat_falloc_first, dat_swalloc_single, dat_swalloc]
    dat = np.log10(dat) - 2
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 4), 10
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
    hatch_vec = [None] * (num_types - 1) + [hatch_def[0]]

    width, ggap, lgap = 0.35, 0.5, 0
    width_fig = ggap + ((width + lgap) * num_types + ggap - lgap) * num_bars
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar(ggap + ((width + lgap) * num_types + ggap - lgap) * i + (width + lgap) * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    xticks_fig = [ggap + ((width + lgap) * num_types - lgap + ggap) * i + ((width + lgap) * num_types - lgap) / 2 for i in range(num_bars)]

    # ax.set_yscale('log')
    ax.set_xticks(xticks_fig)
    # ax.set_xticks([(width * num_types + ggap) * i + (width*3/2 + lgap[1]/2) for i in range(num_bars)])
    ax.set_xticklabels([256 * 2 ** x for x in range(7)],  fontsize=12)
    ax.set_ylabel("时间 (ms)")
    ax.set_xlabel('驻留内存数量')
    ax.set_ylim(0, 4)
    ax.set_yticks(range(5))
    ax.set_yticklabels(["$10^{" + str(x+2) + "}$" for x in range(5)])
    # ax.set_ylim(0, 300)
    types = ['malloc', 'freelist', 'swalloc_single', 'swalloc']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles, loc="upper center", ncol=4, bbox_to_anchor=(0.5, 1.02))

    # plt.show()
    fig.savefig(dirbase + 'random_test.pdf', bbox_inches='tight')


# draw_malloc_random_test()

def draw_malloc_parsec_test():
    dat_malloc = [11.1821, 23.8899, 4437.13, 1286.14, 48.3999, 118.26, 7.823, 636.968, 1071.3, 16.1309, 520.781, 130.97, 7.81494]
    dat_falloc_first = [67.615, 585.271, 10000000, 2118240.0, 103.772, 4789.6, 68.0879, 91.144, 114193.0, 94.9861, 58196.9, 30567.3, 86.4648]
    dat_swalloc_single = [55.3269, 165.623, 34558.5, 2786.61, 66.843, 1199.62, 56.855, 56.7681, 10671.5, 69.1221, 2358.19, 586.942, 55.5291]
    dat_swalloc = [10.6538, 24.8899, 5245.67, 859.311, 13.686, 135.48, 10.6499, 12.2202, 1564.81, 12.627, 756.195, 159.999, 10.6301]
    dat = [dat_malloc, dat_falloc_first, dat_swalloc_single, dat_swalloc]
    dat_prog = ['blackscholes', 'bodytrack', 'canneal', 'dedup', 'facesim', 'ferret', 'fluidanimate', 'freqmine', 'raytrace', 'streamcluster', 'swaptions', 'vips', 'x264']

    dat = np.log10(dat)
    num_bars, num_types = len(dat[0]), len(dat)

    sz, fontsz = (10, 3), 10
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

    hatch_vec = [None] * (num_types - 1) + [hatch_def[0]]

    width, ggap, lgap = 0.35, 0.5, 0
    width_fig = ggap + ((width + lgap) * num_types + ggap - lgap) * num_bars
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar(ggap + ((width + lgap) * num_types + ggap - lgap) * i + (width + lgap) * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    xticks_fig = [ggap + ((width + lgap) * num_types - lgap + ggap) * i + ((width + lgap) * num_types - lgap) / 2 for i in range(num_bars)]

    ax.set_xlim(0, width_fig)
    ax.set_xticks(xticks_fig)
    ax.set_xticklabels([prog for prog in dat_prog], rotation=30, fontsize=12)
    ax.set_ylabel("时间 (ms)")
    ax.set_xlabel('测试程序')
    ax.set_ylim(0, 6)
    ax.set_yticks(range(7))
    ax.set_yticklabels(["$10^{" + str(x) + "}$" for x in range(7)])
    # ax.set_ylim(0, 300)
    types = ['malloc', 'freelist', 'swalloc_single', 'swalloc']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles, bbox_to_anchor=(0.5, 1.25), loc='upper center', ncol=4)

    plt.show()
    fig.savefig(dirbase + 'parsec_test.pdf', bbox_inches='tight')


# draw_malloc_parsec_test()


def draw_malloc_bgl_test():
    dat_malloc = [438277, 412169]
    dat_malloc_e2e = [438277 + 5602.28, 412169 + 4708.18]
    dat_falloc_first = [111.131, 7.70801]
    dat_falloc_best = [305.559, 16.9231]
    dat_swalloc = [19.2852, 5.43481]
    dat_swalloc_e2e = [19.2852 + 5602.28, 5.43481 + 4708.18]
    dat_swalloc_single = [28.5229, 10.9951]

    dat_fig1 = np.log10(np.array([dat_malloc, dat_falloc_best, dat_swalloc_single, dat_swalloc]) / 5)
    dat_fig2 = np.log10(np.array([dat_malloc_e2e, dat_swalloc_e2e]) / 5)

    sz, fontsz = (10, 4), 10
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
    width, ggap, lgap = 0.35, 0.5, 0

    num_types = len(dat_fig1)
    num_bars = len(dat_fig1[0])
    color_vec = color_def[:num_types]
    hatch_vec = [None] * (num_types-1) + [hatch_def[0]]
    dat = dat_fig1
    width_fig1 = ggap + ((width + lgap) * num_types + ggap - lgap) * num_bars
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar(ggap + ((width + lgap) * num_types + ggap - lgap) * i + (width + lgap) * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    xticks_fig1 = [ggap + ((width + lgap) * num_types - lgap + ggap) * i + ((width + lgap) * num_types - lgap) / 2 for i in range(num_bars)]

    num_types = len(dat_fig2)
    num_bars = len(dat_fig2[0])
    color_vec = [color_def[0], color_vec[3]]
    hatch_vec = [None] * (num_types-1) + [hatch_def[0]]
    dat = dat_fig2
    width_fig2 = ggap + ((width + lgap) * num_types + ggap - lgap) * num_bars
    for i in range(num_bars):
        for j in range(num_types):
            ax.bar(width_fig1 + ggap + ((width + lgap) * num_types + ggap - lgap) * i + (width + lgap) * j + width / 2,
                dat[j][i], width, color=color_vec[j], hatch=hatch_vec[j], edgecolor='black')
    xticks_fig2 = [ggap + ((width + lgap) * num_types - lgap + ggap) * i + ((width + lgap) * num_types - lgap) / 2 for i in range(num_bars)]

    color_vec = color_def[:4]
    hatch_vec = [None] * (4-1) + [hatch_def[0]]

    # ax.set_yscale('log')
    ax.set_xlim(0, width_fig1 + width_fig2)
    ax.set_xticks(np.hstack((np.array(xticks_fig1), np.array(xticks_fig2) + width_fig1)))
    # ax.set_xlim(0, ((width + lgap) * num_types - lgap + ggap) * num_bars + ggap)
    # ax.set_xticks([ggap + ((width + lgap) * num_types - lgap + ggap) * i + ((width + lgap) * num_types - lgap) / 2
    #                for i in range(num_bars)])
    ax.set_xticklabels(["BGL-Alloc 3072", "BGL-Alloc 6144", "BGL 3072", "BGL 6144"], fontsize=12)
    ax.set_ylabel("Time (ms)")
    ax.set_ylim(0, 6)
    ax.set_yticks(range(7))
    ax.set_yticklabels("$10^" + str(x) + "$" for x in range(7))
    ax.vlines(ymin=0, ymax = 6, x = width_fig1, colors = "black", linewidth=1)
    types = ['malloc', 'freelist', 'swalloc_single', 'swalloc']
    num_type = len(types)
    legend_handles = [mpatches.Patch(
        facecolor=color_vec[i], edgecolor='black', hatch=hatch_vec[i], label=types[i]) for i in range(num_type)]
    plt.legend(handles=legend_handles, ncol=4, loc="upper center", bbox_to_anchor=(0.5, 1.2))

    # plt.show()
    fig.savefig(dirbase + 'bgl_test.pdf', bbox_inches='tight')


# draw_malloc_bgl_test()
