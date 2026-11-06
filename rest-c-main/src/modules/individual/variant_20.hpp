#pragma once

#include <string>
#include <cmath>
#include "../../json.hpp"

namespace TaskVariant {

    struct Input {
        double contract_value_foreign_currency;
        double current_fx_rate;
        double forecasted_volatility_percentage;
        double hedging_cost_percentage;
    };

    struct Output {
        double contract_value_local_currency;
        double maximum_value_at_risk;
        double net_value_with_hedging;
    };

    inline std::string validate(const Input& in) {
        if (in.contract_value_foreign_currency <= 0.0)
            return "contract_value_foreign_currency must be > 0.0";
        if (in.current_fx_rate <= 0.0001)
            return "current_fx_rate must be > 0.0001";
        if (in.forecasted_volatility_percentage < 0.1 || in.forecasted_volatility_percentage > 200.0)
            return "forecasted_volatility_percentage must be in range [0.1, 200.0]";
        if (in.hedging_cost_percentage < 0.0 || in.hedging_cost_percentage > 50.0)
            return "hedging_cost_percentage must be in range [0.0, 50.0]";
        return "";
    }

    inline Output calculate(const Input& in) {
        Output out{};
        out.contract_value_local_currency = in.contract_value_foreign_currency * in.current_fx_rate;
        out.maximum_value_at_risk = out.contract_value_local_currency * (in.forecasted_volatility_percentage / 100.0);
        out.net_value_with_hedging = out.contract_value_local_currency - (out.contract_value_local_currency * (in.hedging_cost_percentage / 100.0));
        return out;
    }

    inline Input parse(const std::string& body) {
        Input in{};

        auto get_d = [&](const std::string& key) -> double {
            auto p = body.find("\"" + key + "\"");
            if (p == std::string::npos) return 0.0;
            p = body.find(':', p);
            if (p == std::string::npos) return 0.0;
            try { return std::stod(body.substr(p + 1)); }
            catch (...) { return 0.0; }
        };

        in.contract_value_foreign_currency  = get_d("contract_value_foreign_currency");
        in.current_fx_rate                  = get_d("current_fx_rate");
        in.forecasted_volatility_percentage = get_d("forecasted_volatility_percentage");
        in.hedging_cost_percentage          = get_d("hedging_cost_percentage");
        return in;
    }

    inline std::string to_json(const Output& out) {
        JObj o;
        o.num("contract_value_local_currency", out.contract_value_local_currency);
        o.num("maximum_value_at_risk", out.maximum_value_at_risk);
        o.num("net_value_with_hedging", out.net_value_with_hedging);
        return o.str();
    }

} // namespace TaskVariant