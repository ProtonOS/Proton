// Copyright (c) 2009 DotNetAnywhere
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Compat.h"
#include "Sys.h"

#include "System.Runtime.CompilerServices.RuntimeHelpers.h"

#include "MetaData.h"
#include "Types.h"
#include "Heap.h"
#include "Type.h"
#include "System.Array.h"

tAsyncCall* System_Runtime_CompilerServices_InitializeArray(PTR pThis_, PTR pParams, PTR pReturnValue) {
	HEAP_PTR pArray;
	PTR pRawData;
	tMD_TypeDef *pArrayTypeDef;
	PTR pElements;
	U32 arrayLength;

	pArray = ((HEAP_PTR*)pParams)[0];
	pRawData = ((PTR*)pParams)[1];
	pArrayTypeDef = Heap_GetType(pArray);
	arrayLength = SystemArray_GetLength(pArray);
	pElements = SystemArray_GetElements(pArray);
	memcpy(pElements, pRawData, pArrayTypeDef->pArrayElementType->arrayElementSize * arrayLength);

	return NULL;
}
