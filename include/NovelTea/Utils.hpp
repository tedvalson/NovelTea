#ifndef NOVELTEA_UTILS_HPP
#define NOVELTEA_UTILS_HPP

#define ADD_ACCESSOR(type, name, var) \
	void set##name(const type & arg){var = arg;} \
	const type & get##name() const {return var;}

#define SET_ALPHA(getFunc, setFunc, maxValue) \
	color = getFunc(); \
	color.a = std::max(std::min(newValues[0] * maxValue / 255.f, 255.f), 0.f); \
	setFunc(color);

namespace NovelTea
{

} // namespace NovelTea

#endif // NOVELTEA_UTILS_HPP
