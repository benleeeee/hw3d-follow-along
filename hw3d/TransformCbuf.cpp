#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	parent(parent)
{

}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	vcbuf.Update(gfx,
		DirectX::XMMatrixTranspose(	//Need to make sure it's transposed or matrix will be wrong
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	vcbuf.Bind(gfx);
}
