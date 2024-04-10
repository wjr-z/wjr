import os
import sys
import re
import matplotlib.pyplot as plt
import json


def get_file_content(file):
    with open(file, "r") as f:
        return f.read()


def get_json(content):
    data = json.loads(content)
    result = []
    count = 0
    map = {}
    for item in data["benchmarks"]:
        name = item["name"]
        real_time = item["real_time"]
        idx = 0
        if name.startswith("wjr_"):
            name = name[4:]
        elif name.startswith("fallback_"):
            name = name[9:]
            idx = 1
        elif name.startswith("gmp_"):
            name = name[4:]
            idx = 2
        else:
            print("unknown prefix: ", name)
            exit(1)
        tmp = 0
        if name in map:
            tmp = map[name]
        else:
            map[name] = count
            tmp = count
            count += 1
        result.append((name, real_time, idx, tmp))
    return result


def call_scroll(event, fig):
    axtemp = event.inaxes
    # caculate the xlim and ylim after zooming
    if axtemp:
        x_min, x_max = axtemp.get_xlim()
        y_min, y_max = axtemp.get_ylim()
        w = x_max - x_min
        h = y_max - y_min
        curx = event.xdata
        cury = event.ydata
        curXposition = (curx - x_min) / w
        curYposition = (cury - y_min) / h
        # Zoom the figure for 1.1 times
        if event.button == "down":
            w = w * 1.1
            h = h * 1.1
        elif event.button == "up":
            w = w / 1.1
            h = h / 1.1
        newx = curx - w * curXposition
        newy = cury - h * curYposition
        axtemp.set(xlim=(newx, newx + w))
        axtemp.set(ylim=(newy, newy + h))
        fig.canvas.draw_idle()  # drawing


def push_none(data, already):
    for i in range(3):
        if already[i] == 0:
            data[i].append(None)


def plot(result):
    result.sort(key=lambda x: (x[3], x[2]))

    while True:
        suffix = input("input suffix expression: ")

        data = ([], [], [])
        name = []
        already = [0, 0, 0]

        for item in result:
            xname = item[0]
            if re.search(suffix, xname):
                idx = item[2]
                if len(name) == 0 or name[-1] != xname:
                    if len(name) != 0:
                        push_none(data, already)
                        already = [0, 0, 0]
                    name.append(xname)
                data[idx].append(item[1])
                already[idx] = 1

        if len(name) != 0:
            push_none(data, already)
        else:
            print("no data found")
            continue

        fig, ax = plt.subplots()

        ax.plot(name, data[0], label="wjr", marker="o")
        ax.plot(name, data[1], label="fallback", marker="x")
        ax.plot(name, data[2], label="gmp", marker="*")

        plt.xticks(range(len(name)), name, rotation=90)
        plt.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.05)
        fig.tight_layout()
        fig.autofmt_xdate()

        fig.canvas.mpl_connect(
            "scroll_event", lambda event: call_scroll(event, fig)
        )  # Event mouse wheel

        plt.legend()
        plt.show()


def format(value, precision):
    return (value >= 0 and "+" or "") + "{:.{precision}f}%".format(
        value, precision=precision
    )


def print_result(result, name, file, start_color, end_color):
    print(
        start_color + name + str(len(result)).rjust(5) + " cases" + end_color,
        file=file,
    )

    if len(result) < 16:
        if len(result) == 0:
            return
        print(result, file=file)
    else:
        print(result[:16] + ["..."], file=file)


# only compare wjr_ result
def compare(old, new, output):
    map = {}
    for item in old:
        if item[2] != 0:
            continue
        map[item[0]] = (item[1], None)

    for item in new:
        if item[2] != 0:
            continue
        if item[0] in map:
            map[item[0]] = (map[item[0]][0], item[1])
        else:
            map[item[0]] = (None, item[1])

    if output is None:
        mylog = None
        END_COLOR = "\033[0m"
        COLOR_RED = "\033[31m"
        COLOR_YELLOW = "\033[33m"
        COLOR_PUR = "\033[35m"
        COLOR_BLUE = "\033[36m"
        COLOR_GREEN = "\033[32m"
    else:
        mylog = open(output, "w")
        END_COLOR = ""
        COLOR_RED = ""
        COLOR_YELLOW = ""
        COLOR_PUR = ""
        COLOR_BLUE = ""
        COLOR_GREEN = ""

    print(
        "Benchmark".ljust(40),
        "Old".rjust(20),
        "New".rjust(20),
        "Delta".rjust(20),
        file=mylog,
    )

    print("-" * 93, file=mylog)

    better2 = []
    worse2 = []
    better5 = []
    worse5 = []

    for name, item in map.items():
        if item[0] is None or item[1] is None:
            continue

        delta = (item[1] - item[0]) / item[0] * 100

        print(
            COLOR_GREEN + name.ljust(40) + END_COLOR,
            "{:.2f}".format(item[0]).rjust(20),
            "{:.2f}".format(item[1]).rjust(20),
            file=mylog,
            end="",
        )

        # 保留两位小数，正数加上+，否则加上负号
        str_delta = format(delta, 2)

        if abs(delta) >= 5:
            if delta > 0:
                color = COLOR_RED
                worse5.append(name)
            else:
                color = COLOR_BLUE
                better5.append(name)
            print(
                color + str_delta.rjust(20) + END_COLOR,
                file=mylog,
            )
        elif abs(delta) >= 2:
            if delta > 0:
                color = COLOR_YELLOW
                worse2.append(name)
            else:
                color = COLOR_PUR
                better2.append(name)
            print(
                color + str_delta.rjust(20) + END_COLOR,
                file=mylog,
            )
        else:
            print(
                str_delta.rjust(20),
                file=mylog,
            )

    print("-" * 93, file=mylog)

    print_result(
        worse5, "Performance has decreased by at least 5%:", mylog, COLOR_RED, END_COLOR
    )

    print("-" * 93, file=mylog)

    print_result(
        worse2,
        "Performance has decreased by at least 2%:",
        mylog,
        COLOR_YELLOW,
        END_COLOR,
    )

    print("-" * 93, file=mylog)

    print_result(
        better2,
        "Performance has increased by at least 2%:",
        mylog,
        COLOR_PUR,
        END_COLOR,
    )

    print("-" * 93, file=mylog)

    print_result(
        better5,
        "Performance has increased by at least 5%:",
        mylog,
        COLOR_BLUE,
        END_COLOR,
    )

    print("-" * 93, file=mylog)

    if mylog is not None:
        mylog.close()


def main():
    length = len(sys.argv)
    if length < 2 or length > 4:
        print("usage: python compare.py file [file] [output]")
        return

    if length == 3 or length == 4:
        old_file = sys.argv[1]
        new_file = sys.argv[2]
        output_file = None

        if length == 4:
            output_file = sys.argv[3]

        old_content = get_file_content(old_file)
        new_content = get_file_content(new_file)
        old_result = get_json(old_content)
        new_result = get_json(new_content)
        compare(old_result, new_result, output_file)
        return

    file = sys.argv[1]
    content = get_file_content(file)
    result = get_json(content)
    plot(result)


if __name__ == "__main__":
    main()
