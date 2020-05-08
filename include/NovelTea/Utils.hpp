#ifndef NOVELTEA_UTILS_HPP
#define NOVELTEA_UTILS_HPP

#define ADD_ACCESSOR(type, name, var) \
	void set##name(const type & arg){var = arg;} \
	const type & get##name() const {return var;}

namespace NovelTea
{

} // namespace NovelTea

#endif // NOVELTEA_UTILS_HPP
