#include "order_confirmation.h"

namespace mb
{
	order_confirmation::order_confirmation(
		std::string orderId,
		order_status orderStatus,
		double originalQty,
		double filledQty,
		double price)
		: 
		_orderId{ std::move(orderId) },
		_orderStatus{ orderStatus },
		_originalQty{ originalQty },
		_filledQty{ filledQty },
		_price{ price }
	{}
}