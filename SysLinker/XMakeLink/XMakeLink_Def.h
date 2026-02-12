#pragma once

#include <unordered_map>

#include <immintrin.h>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

// CPU AVX사용
#define __AVX2__

#define __MAKE_LINK_LOG__ 0

#define USE_PARKING_SUBSTRING 0


#define N_INPUTID_MAX				64 * 32 * 4 * 256	
#define	MAX_LOGIC_ITEM_CNT	99
#define	MAX_LOGIC_PRIORITY	99



enum
{
	L_OP_NONE = 0,
	L_OP_EQUALEQUAL = 1, // ==
	L_OP_LESSEQUAL = 2, // X <= Y : X IS LESS
	L_OP_GREATEREQUAL = 3, //X >= Y
	L_OP_LESS = 4, // X < Y
	L_OP_GREATER = 5, // X > Y
};


enum
{
	MATCH_GROUND = 1,
	MATCH_UNDER = 2,
	MATCH_PARK = 3
};



#pragma pack(push,1)

struct ST_LOGICAL_ADDR
{
	DWORD		dwDevID : 8; // 0 ~ 255
	DWORD		dwChnID : 2; // 0 ~ 3
	DWORD		dwUnitID : 6; // 0 ~ 63
	DWORD		dwFacpID : 5; // 0 ~ 31
	DWORD		dwResrv : 11; // 0 ~ 15
};


union SU_LOGICAL_ADDR
{
	ST_LOGICAL_ADDR stAddr;
	DWORD			dwAddr;
};

struct ST_PHYSICAL_ADDR
{
	DWORD		dwFacpNo : 7; // 0 ~ 127
	DWORD		dwUnitNo : 8; // 0 ~ 255
	DWORD		dwChnNo : 4; // 0 ~ 15
	DWORD		dwDevNo : 9; // 0 ~ 512
	DWORD		dwResrv : 4; // 0 ~ 15
};

union SU_PHYSICAL_ADDR
{
	ST_PHYSICAL_ADDR stAddr;
	DWORD			dwAddr;
};

struct ST_LOGIC_LOC
{
	DWORD		dwRoom : 7; // 0 ~ 127
	DWORD		dwFloor : 7; // 0 ~ 127
	DWORD		dwStair : 5; // 0 ~ 31
	DWORD		dwBType : 2; // 0 ~ 4
	DWORD		dwBuild : 7; // 0 ~ 127
	DWORD		dwResrv : 4;
};

union SU_LOGIC_LOC
{
	ST_LOGIC_LOC	stLoc;
	DWORD			dwLoc;
};


#pragma pack(pop)

class CXDataDev;
class CXDataLink;
typedef std::unordered_map<DWORD,CXDataDev*> CXMapDev;
typedef std::unordered_map<DWORD,CXDataLink*> CXMapLink;
typedef std::map<CString,int> CXLocStrMap;

inline void cpuid(int out[4],int op,int sub = 0) {
#if defined(_MSC_VER)
	__cpuidex(out,op,sub);
#else
	__cpuid_count(op,sub,out[0],out[1],out[2],out[3]);
#endif
}

inline uint64_t xgetbv(unsigned int x) {
#if defined(_MSC_VER)
	return _xgetbv(x);
#else
	uint32_t eax,edx;
	__asm__ volatile("xgetbv" : "=a"(eax),"=d"(edx) : "c"(x));
	return ((uint64_t)edx << 32) | eax;
#endif
}

inline bool CPU_AVX2() {
	int r[4];
	cpuid(r,1);
	if(!(r[2] & (1 << 27))) return false; // OSXSAVE
	if((xgetbv(0) & 0x6) != 0x6) return false;
	cpuid(r,7);
	return (r[1] & (1 << 5)) != 0;
}

inline bool CPU_AVX512() {
	int r[4];
	cpuid(r,1);
	if(!(r[2] & (1 << 27))) return false;
	if((xgetbv(0) & 0xE6) != 0xE6) return false;
	cpuid(r,7);
	bool f = (r[1] & (1 << 16)) != 0;
	bool bw = (r[1] & (1 << 30)) != 0;
	return f && bw;
}


/************************************************************************************
 * GFX_SubSetScalar
 *  @brief 
 *		check group subset of input
 *		'in'안에 gr이 있는가 검사
 *		gr ⊆ in 인가?
 *  @param
 *		gr : 패턴 아이템이 설정된 비트
 *		in : 디바이스의 연동출력 비트
 *  @return  
 * \date 2026/1/19 11:00:24
 ************************************************************************************/
inline bool GFX_SubSetScalar(const uint64_t* in,const uint64_t* gr,size_t n) {
	for(size_t i = 0; i < n; i++)
		if(gr[i] & (~in[i])) return false;
	return true;
}

/************************************************************************************
 * GFX_SubSet_AVX2
 *  @brief 
 *		(~a) & b
 *		gr에 1로 설정된 모든 비트가 in에도 반드시 1이어야 한다
 *		
 *  @param
 *		in : 디바이스의 연동출력 비트
 *		gr : 패턴 아이템이 설정된 비트
 *  @return  return
 * \date 2026/1/19 11:05:57
 ************************************************************************************/
#if defined(__AVX2__)
inline bool GFX_SubSet_AVX2(const uint64_t* in,const uint64_t* gr,size_t n)
{
	size_t i = 0; const size_t step = 4;

	// 4Byte 씩 끈어서 비트 연산
	// 중간에 gr(패턴아이템 비트)가 dev의 연동출력 항목에 없으면 false를 리턴하면서 빠져 나온다.
	for(; i + step <= n; i += step)
	{
		__m256i vin = _mm256_loadu_si256((__m256i*)(in + i));
		__m256i vgr = _mm256_loadu_si256((__m256i*)(gr + i));

		// (~a) & b
		// vin과 vgr이 같으면 v는 0
		__m256i v = _mm256_andnot_si256(vin,vgr);
		
		// 모든 비트가 0이면 TRUE , 
		if(!_mm256_testz_si256(v,v)) return false;
	}

	// 4BYTE 씩 끈은 나면지 부분 체크
	// 만일 5BYTE이면 4BYTE부분은 for 문에서 처리하고 
	// 나머지 GFX_SubSetScalar 이 함수에서 처리한다.
	return GFX_SubSetScalar(in + i,gr + i,n - i);
}

inline void GFX_RemoveBit_AVX2(uint64_t* A,const uint64_t* B,size_t words)
{
	size_t i = 0;
	const size_t step = 4; // 4*64bit = 256bit per AVX2 load

	for(; i + step <= words; i += step)
	{
		__m256i va = _mm256_loadu_si256((const __m256i*)(A + i));
		__m256i vb = _mm256_loadu_si256((const __m256i*)(B + i));
		// XOR 후 AND
		__m256i vx = _mm256_xor_si256(va,vb);
		__m256i vr = _mm256_and_si256(va,vx);
		_mm256_storeu_si256((__m256i*)(A + i),vr);

		// 최적화: XOR 없이 _mm256_andnot_si256 사용 가능
		// vr = _mm256_andnot_si256(vb, va);
	}

	// 남은 스칼라 처리
	for(; i < words; ++i)
		A[i] = A[i] & (A[i] ^ B[i]);
}

inline void GFX_MemSet_AVX2(uint64_t* arr,size_t words,uint64_t val)
{
	size_t i = 0;
	__m256i v = _mm256_set1_epi64x(val); // 4 uint64_t = val

	for(; i + 4 <= words; i += 4)
		_mm256_storeu_si256((__m256i*)(arr + i),v);

	for(; i < words; ++i)
		arr[i] = val;
}

inline void GFX_ORBit_AVX2(uint64_t* A,const uint64_t* B,size_t words)
{
	size_t i = 0;
	const size_t step = 4; // 4×64bit = 256bit per AVX2 register

	for(; i + step <= words; i += step)
	{
		__m256i va = _mm256_loadu_si256((__m256i const*)(A + i));
		__m256i vb = _mm256_loadu_si256((__m256i const*)(B + i));

		__m256i vr = _mm256_or_si256(va,vb);

		_mm256_storeu_si256((__m256i*)(A + i),vr);
	}

	// 남은 나머지 처리
	for(; i < words; ++i)
	{
		A[i] |= B[i];
	}
}
#endif

#if defined(__AVX512F__)
inline bool GFX_SubSet_AVX512(const uint64_t* in,const uint64_t* gr,size_t n) {
	size_t i = 0; const size_t step = 8;
	for(; i + step <= n; i += step) {
		__m512i vin = _mm512_loadu_si512(in + i);
		__m512i vgr = _mm512_loadu_si512(gr + i);
		__m512i v = _mm512_andnot_si512(vin,vgr);
		if(_mm512_test_epi64_mask(v,v)) return false;
	}
	return subset_scalar(in + i,gr + i,n - i);
}
inline void GFX_RemoveBit_AVX512(uint64_t* A,const uint64_t* B,size_t words)
{
	size_t i = 0;
	const size_t step = 8; // 8*64bit = 512bit per AVX-512 load

	for(; i + step <= words; i += step)
	{
		__m512i va = _mm512_loadu_si512(A + i);
		__m512i vb = _mm512_loadu_si512(B + i);
		__m512i vx = _mm512_xor_si512(va,vb);
		__m512i vr = _mm512_and_si512(va,vx);
		_mm512_storeu_si512(A + i,vr);

		// 최적화: _mm512_andnot_si512(vb, va) 로 XOR 없이 가능
		// vr = _mm512_andnot_si512(vb, va);
	}

	for(; i < words; ++i)
		A[i] = A[i] & (A[i] ^ B[i]);
}

inline void GFX_MemSet_AVX512(uint64_t* arr,size_t words,uint64_t val)
{
	size_t i = 0;
	__m512i v = _mm512_set1_epi64(val);

	for(; i + 8 <= words; i += 8)
		_mm512_storeu_si512(arr + i,v);

	for(; i < words; ++i)
		arr[i] = val;
}
#endif


inline void GFX_FindSetBit(uint64_t word,uint32_t baseIndex,std::vector<uint32_t>& out)
{
	while(word)
	{
		//uint64_t lsb = word & -word;        // lowest set bit
		uint64_t lsb = word & (~word + 1);
		uint32_t bitIndex = _tzcnt_u64(word); // requires BMI1 (AVX2 CPU has it)
		out.push_back(baseIndex + bitIndex);
		word ^= lsb; // clear lowest bit
	}
}

inline std::vector<uint32_t> GFX_ExtractSetBit(const uint64_t* data,size_t words)
{
	std::vector<uint32_t> out;
	out.reserve(256); // 적당한 초기 값 (필요시 자동 증가)

	for(size_t i = 0; i < words; i++)
	{
		uint64_t w = data[i];
		if(w != 0)
		{
			uint32_t base = (uint32_t)(i * 64);
			GFX_FindSetBit(w,base,out);
		}
	}
	return out;
}
// --- Dispatcher ---
typedef bool(*GFX_SubSet_Fn)(const uint64_t*,const uint64_t*,size_t);
extern GFX_SubSet_Fn g_subset;

inline void GFX_InitSimd() {
	//if(CPU_AVX512()) { g_subset = GFX_SubSet_AVX512; return; }
	if(CPU_AVX2()) { g_subset = GFX_SubSet_AVX2;   return; }
	g_subset = GFX_SubSetScalar;
}


extern CXLocStrMap	g_MapIdxBuild;
extern CXLocStrMap	g_MapIdxBtype;
extern CXLocStrMap	g_MapIdxFloor;
extern CXLocStrMap	g_MapIdxStair;
extern CXLocStrMap	g_MapIdxRoom;

// [2025/8/1 11:12:00 KHS] 
// 로직 처리를 위해 Define
// #define NAME_PARKING_KOR	L"주차장"	
// #define NAME_PARKING_ENG	L"PARKING"	
#if USE_PARKING_SUBSTRING
	#define PARKING_SUBSTRING		L"@PK"
#endif

extern BOOL Less_than(int a,int b);
extern BOOL Greater_than(int a,int b);
extern BOOL Less_equal(int a,int b);
extern BOOL Greater_equal(int a,int b);
extern BOOL Equal_equal(int a,int b);
extern BOOL None_none(int a,int b);

extern UINT GF_SetAddr(int nFacpID,int nUnitID,int nChnID,int nDevID);
extern UINT GF_GetFacpID(DWORD dwAddr);
extern UINT GF_GetUnitID(DWORD dwAddr);
extern UINT GF_GetChnID(DWORD dwAddr);
extern UINT GF_GetDevID(DWORD dwAddr);

extern BOOL GF_SetBit(uint64_t * pBuf,int nIdx,BOOL bValue);
extern BOOL GF_GetBit(uint64_t * pBuf,int nIdx);

extern void WriteXMakeLinkLog(const WCHAR * szFmt,...);

#define BITCNT_FACPITEM		((32 * 62 * 4 * 256) + 63)/64