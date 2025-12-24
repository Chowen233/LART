#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <chrono>

class progress_bar {
  public:
    progress_bar(int total_steps) :
        total_steps(total_steps), start(std::chrono::high_resolution_clock::now()) {}

    void update(int current_step) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - start);
        double elapsed_sec = elapsed.count();
        double avg_per_line = elapsed_sec / double(current_step);
        double remaining_sec = avg_per_line * double(total_steps - current_step);

        // progress bar
        const int BAR_WIDTH = 50;
        int filled = int(std::round(double(current_step) / double(total_steps) * BAR_WIDTH));
        if (filled < 0) filled = 0;
        if (filled > BAR_WIDTH) filled = BAR_WIDTH;

        std::ostringstream oss;
        oss << cursor_up << "\rRendering: [" << red << bold;
        for (int b = 0; b < BAR_WIDTH; ++b) {
            if (b < filled) oss << '=';
            else oss << ' ';
        }
        oss << reset << "] ";

        int percent = int(std::round(100.0 * double(current_step) / double(total_steps)));
        if (percent < 0) percent = 0;
        if (percent > 100) percent = 100;
        oss << percent << "%\n";

        auto fmt_time = [](double sec)->std::string {
            if (sec < 0) sec = 0;
            int s = int(std::round(sec));
            int m = s / 60;
            s = s % 60;
            std::ostringstream tss;
            if (m > 0) tss << m << "m ";
            tss << s << "s remaining         ";
            return tss.str();
            };

        oss << fmt_time(remaining_sec);
        std::clog << oss.str() << std::flush;
    }

    void end(double average_sample_count) {
        auto end = std::chrono::high_resolution_clock::now();
        auto total = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

        std::clog << cursor_up << "\rRendering: [" << green << bold;
        for (int i = 0; i < 50; ++i) std::clog << '=';
        std::clog << reset << "] 100% Done.\nTotal time: " << int(total.count()) << "s          \n";

        std::clog << "Average sample count = " << average_sample_count << "\n";
    }

  private:
    std::chrono::high_resolution_clock::time_point start;
    int total_steps;
    int current_step;

    static constexpr const char* red = "\033[31m";
    static constexpr const char* green = "\033[32m";
    static constexpr const char* bold = "\033[1m";
    static constexpr const char* reset = "\033[0m";
    static constexpr const char* cursor_up = "\033[1F";  // ÉÏÒÆÒ»ÐÐ
};

#endif