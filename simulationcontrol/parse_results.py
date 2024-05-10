import os
import pandas as pd
from tabulate import tabulate
from resultlib import *


def main():
    headers = [
        "date",
        "config",
        "tasks",
        #'sim. time (ns)',
        "avg resp time (ns)",
        #'resp times (ns)',
        #'peak power / thread (W)',
        "avg power (W)",
        #'energy (J)',
        "avg temperature (C)",
        #"utilization",
    ]
    rows = []
    runs = sorted(list(get_runs()))

    configs = []
    tasks_ = []
    avg_resp_time = []
    avg_power = []
    avg_temp = []
    util = []

    for run in runs:
        if has_properly_finished(run):
            config = get_config(run)
            # energy = get_energy(run)
            tasks = get_tasks(run)
            if len(tasks) > 40:
                tasks = tasks[:37] + "..."
            rows.append(
                [
                    get_date(run),
                    config,
                    tasks,
                    #'{:,}'.format(get_total_simulation_time(run)),
                    "{:,}".format(get_average_response_time(run)),
                    #'  '.join('{:,}'.format(r) for r in get_individual_response_times(run)),
                    #'{:.2f}'.format(get_peak_power_consumption_single_thread(run)),
                    "{:.2f}".format(get_average_power_consumption(run)),
                    #'{:.2f}'.format(energy) if energy != '-' else '-',
                    "{:.2f}".format(get_average_peak_temperature(run)),
                    "{:.2f}".format(get_average_system_utilization(run)),
                ]
            )
            configs.append(rows[-1][1])
            tasks_.append(rows[-1][2])
            avg_resp_time.append(int(rows[-1][3].replace(',','')))
            avg_power.append(rows[-1][4])
            avg_temp.append(rows[-1][5])
            util.append(rows[-1][6])

    print(tabulate(rows, headers=headers))
    df = pd.DataFrame(
        {
            "config": configs,
            "tasks": tasks_,
            "avg resp time (ns)": avg_resp_time,
            "avg power (W)": avg_power,
            "avg temperature (C)": avg_temp,
            "utilization rate (%)": util,
        }
    )
    df.to_csv("results.csv", index=False)


if __name__ == "__main__":
    main()