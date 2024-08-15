#ifndef ChineseG2pPRIVATE_H
#define ChineseG2pPRIVATE_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "ChineseG2p_p.h"
#include "ToneConverter.h"

namespace Pinyin
{
    class ChineseG2pPrivate final {
    public:
        explicit ChineseG2pPrivate(std::string language, ToneConverter *toneConverter);
        virtual ~ChineseG2pPrivate();

        void init();

        bool initialized = false;

        ChineseG2p *q_ptr{};

        std::unordered_map<u8string, u8string> phrases_map;
        std::unordered_map<u8string, u8stringlist> phrases_dict;
        std::unordered_map<u8string, u8stringlist> word_dict;
        std::unordered_map<u8string, u8string> trans_dict;

        std::string m_language;
        ToneConverter *m_toneConverter;

        inline bool isPolyphonic(const u8string &text) const {
            return phrases_map.find(text) != phrases_map.end();
        }

        inline u8string tradToSim(const u8string &text) const {
            const auto it = trans_dict.find(text);
            return it != trans_dict.end() ? it->second : text;
        }

        inline u8string toneConvert(const u8string &pinyin, int style) const {
            return m_toneConverter->convert(pinyin, style);
        }

        inline u8stringlist toneConvert(const u8stringlist &pinyin, int style) const {
            u8stringlist tonePinyin;
            tonePinyin.reserve(pinyin.size());
            for (const u8string &p : pinyin) {
                tonePinyin.push_back(toneConvert(p, style));
            }
            return tonePinyin;
        }

        inline std::vector<std::string> stdToneConvert(const std::vector<std::string> &pinyin, int style) const {
            std::vector<std::string> tonePinyin;
            tonePinyin.reserve(pinyin.size());
            for (const std::string &p : pinyin) {
                tonePinyin.emplace_back(toneConvert(u8string(p), style).c_str());
            }
            return tonePinyin;
        }

        inline u8stringlist getDefaultPinyin(const u8string &hans, int style = 0) const {
            const auto it = word_dict.find(tradToSim(hans));
            if (it == word_dict.end())
                return {hans};

            u8stringlist candidates = it->second;
            u8stringlist toneCandidates;
            toneCandidates.reserve(candidates.size());

            std::unordered_set<u8string> seen;

            for (u8string &pinyin : candidates) {
                const auto tarPinyin = toneConvert(pinyin, style);
                if (seen.insert(tarPinyin).second) {
                    toneCandidates.push_back(tarPinyin);
                }
            }

            if (toneCandidates.empty())
                return {hans};
            return toneCandidates;
        }

        void zhPosition(const u8stringlist &input, u8stringlist &res, std::vector<int> &positions);
    };
}

#endif // ChineseG2pPRIVATE_H
