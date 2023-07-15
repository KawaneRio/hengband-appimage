#include "util/candidate-selector.h"
#include <algorithm>
#include <iterator>

/*!
 * @brief 候補の選択に使用するシンボルのリスト
 */
const std::array<char, 62> CandidateSelector::i2sym = {
    // clang-format off
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    // clang-format on
};

CandidateSelector::CandidateSelector()
    : prompt(_("選択: ", "Choose: "))
    , start_col(0)
{
    this->set_max_per_page();
}

std::pair<size_t, std::optional<size_t>> CandidateSelector::process_input(char cmd, size_t current_page, size_t page_max)
{
    switch (cmd) {
    case ' ':
        current_page++;
        break;
    case '-':
        current_page += (page_max - 1);
        break;
    default:
        if (auto select_sym_it = std::find(i2sym.begin(), i2sym.end(), cmd);
            select_sym_it != i2sym.end()) {
            const auto idx = static_cast<size_t>(std::distance(i2sym.begin(), select_sym_it));
            return { current_page, idx };
        }
        break;
    }

    if (current_page >= page_max) {
        current_page %= page_max;
    }

    return { current_page, std::nullopt };
}

/*!
 * @brief 1ページに表示する候補の最大数を設定する
 *
 * 引数を省略した場合もしくは設定数が端末の高さより大きい場合は、端末の高さに合わせる
 *
 * @param max 1ページに表示する候補の最大数
 */
void CandidateSelector::set_max_per_page(size_t max)
{
    TERM_LEN term_w, term_h;
    term_get_size(&term_w, &term_h);

    this->max_per_page = std::min<size_t>(max, term_h - 2);
}
