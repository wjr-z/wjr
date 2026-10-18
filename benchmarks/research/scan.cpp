#include <wjr/np/expression_scan.hpp>
namespace np=wjr::np;
extern "C" WJR_NOINLINE void scan1_np(double* out,const double* in,size_t n) {
    using shape=np::dextents<size_t,1>;
    const auto x=np::as_expr(np::tensor_view<const double,shape>(in,n));
    np::cumsum_to(np::tensor_view<double,shape>(out,n),x*2.+1.,np::axes<0>);
}
extern "C" WJR_NOINLINE void scan1_raw(double* out,const double* in,size_t n) {
    double state=0.;
    for(size_t i=0;i!=n;++i) { state+=in[i]*2.+1.;out[i]=state; }
}
extern "C" WJR_NOINLINE void inner_np(double* out,const double* in,size_t rows) {
    using shape=np::extents<size_t,np::dynamic_extent,19>;
    const auto x=np::as_expr(np::tensor_view<const double,shape>(in,rows));
    np::cumsum_to(np::tensor_view<double,shape>(out,rows),x*2.+1.,np::axes<1>);
}
extern "C" WJR_NOINLINE void inner_raw(double* out,const double* in,size_t rows) {
    for(size_t row=0;row!=rows;++row) {
        double state=0.;
        for(size_t col=0;col!=19;++col) {const auto i=row*19+col;state+=in[i]*2.+1.;out[i]=state;}
    }
}
extern "C" WJR_NOINLINE void columns_np(double* out,const double* in,size_t rows,size_t cols) {
    using shape=np::dextents<size_t,2>;
    const auto x=np::as_expr(np::tensor_view<const double,shape>(in,rows,cols));
    np::cumsum_to(np::tensor_view<double,shape>(out,rows,cols),x*2.+1.,np::axes<0>);
}
extern "C" WJR_NOINLINE void columns_naive(double* out,const double* in,size_t rows,size_t cols) {
    for(size_t col=0;col!=cols;++col) {
        double state=0.;
        for(size_t row=0;row!=rows;++row) {const auto i=row*cols+col;state+=in[i]*2.+1.;out[i]=state;}
    }
}
extern "C" WJR_NOINLINE void columns_raw(double* out,const double* in,size_t rows,size_t cols) {
    size_t col=0;
    for(;cols-col>=4;col+=4) {
        double a=0.,b=0.,c=0.,d=0.;
        for(size_t row=0;row!=rows;++row) {
            const auto i=row*cols+col;
            a+=in[i]*2.+1.;b+=in[i+1]*2.+1.;c+=in[i+2]*2.+1.;d+=in[i+3]*2.+1.;
            out[i]=a;out[i+1]=b;out[i+2]=c;out[i+3]=d;
        }
    }
    for(;col!=cols;++col) {
        double state=0.;
        for(size_t row=0;row!=rows;++row) {const auto i=row*cols+col;state+=in[i]*2.+1.;out[i]=state;}
    }
}
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>

template<bool Padded>
WJR_NOINLINE void mapped_scan(double* out,const double* in,const double* bias,size_t rows,size_t cols,size_t pitch) {
    using shape=np::dextents<size_t,2>;
    const auto source=[&] {
        if constexpr(Padded) return np::tensor_view<const double,shape,np::layout_stride>(in,
            np::layout_stride::mapping<shape>(shape(rows,cols),std::array<size_t,2>{pitch,1}));
        else return np::tensor_view<const double,shape>(in,rows,cols);
    }();
    const auto expression=np::map([](double x,double b) noexcept { return (x-b)*1.25; },
        source,np::tensor_view<const double,np::dextents<size_t,1>>(bias,cols));
    np::cumsum_to(np::tensor_view<double,shape>(out,rows,cols),expression,np::axes<0>);
}
extern "C" WJR_NOINLINE void map_np(double* out,const double* in,const double* bias,size_t rows,size_t cols,size_t pitch) {
    mapped_scan<false>(out,in,bias,rows,cols,pitch);
}
extern "C" WJR_NOINLINE void padded_np(double* out,const double* in,const double* bias,size_t rows,size_t cols,size_t pitch) {
    mapped_scan<true>(out,in,bias,rows,cols,pitch);
}
extern "C" WJR_NOINLINE void mapped_raw(double* out,const double* in,const double* bias,size_t rows,size_t cols,size_t pitch) {
    size_t col=0;
    for(;cols-col>=4;col+=4) {
        double a=0.,b=0.,c=0.,d=0.;
        for(size_t row=0;row!=rows;++row) {
            const auto input=row*pitch+col,output=row*cols+col;
            a+=(in[input]-bias[col])*1.25;b+=(in[input+1]-bias[col+1])*1.25;
            c+=(in[input+2]-bias[col+2])*1.25;d+=(in[input+3]-bias[col+3])*1.25;
            out[output]=a;out[output+1]=b;out[output+2]=c;out[output+3]=d;
        }
    }
    for(;col!=cols;++col) {
        double state=0.;
        for(size_t row=0;row!=rows;++row) {state+=(in[row*pitch+col]-bias[col])*1.25;out[row*cols+col]=state;}
    }
}
using mapped_function=void(*)(double*,const double*,const double*,size_t,size_t,size_t);
volatile double sink=0.;
template<class T> T escape(T value) { asm volatile("" : "+r"(value) :: "memory");return value; }
double measure(mapped_function fn,double* out,const double* in,const double* bias,size_t rows,size_t cols,size_t pitch,size_t repeats) {
    fn=escape(fn);out=escape(out);in=escape(in);bias=escape(bias);
    const auto begin=std::chrono::steady_clock::now();
    for(size_t i=0;i!=repeats;++i) {fn(out,in,bias,rows,cols,pitch);asm volatile("":::"memory");}
    if(rows&&cols)sink=out[rows*cols-1];
    return std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-begin).count()/repeats;
}
void fixed_mapped(bool padded) {
    for(auto [rows,cols]: {std::pair<size_t,size_t>{8,9},{64,300},{1024,300},{512,32}}) {
        const auto pitch=cols+(padded?5:0);
        std::vector<double> in(rows*pitch),bias(cols),actual(rows*cols),expected(rows*cols);
        for(size_t i=0;i!=in.size();++i)in[i]=double(int(i%23)-11)*.125;
        for(size_t i=0;i!=cols;++i)bias[i]=double(int(i%7)-3)*.25;
        const auto fn=padded?padded_np:map_np;
        fn(actual.data(),in.data(),bias.data(),rows,cols,pitch);
        mapped_raw(expected.data(),in.data(),bias.data(),rows,cols,pitch);
        if(actual!=expected) {std::fprintf(stderr,"oracle failed\n");std::exit(1);}
        const auto repeats=std::max(size_t(15),size_t(3000000)/(rows*cols));
        std::vector<double> a,b;
        for(size_t trial=0;trial!=11;++trial) {
            if(trial%2) {
                a.push_back(measure(fn,actual.data(),in.data(),bias.data(),rows,cols,pitch,repeats));
                b.push_back(measure(mapped_raw,expected.data(),in.data(),bias.data(),rows,cols,pitch,repeats));
            } else {
                b.push_back(measure(mapped_raw,expected.data(),in.data(),bias.data(),rows,cols,pitch,repeats));
                a.push_back(measure(fn,actual.data(),in.data(),bias.data(),rows,cols,pitch,repeats));
            }
        }
        std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
        std::printf("%s %zux%zu np=%.2f raw4=%.2f ratio=%.3f\n",padded?"padded-map":"map",rows,cols,a[5],b[5],a[5]/b[5]);
    }
}
void mixed() {
    std::vector<std::pair<size_t,size_t>> schedule;
    for(size_t r:{size_t(0),size_t(1),size_t(8),size_t(64),size_t(1024)})
        for(size_t c:{size_t(0),size_t(1),size_t(3),size_t(4),size_t(7),size_t(15),size_t(300)})schedule.push_back({r,c});
    std::mt19937 random(731);std::shuffle(schedule.begin(),schedule.end(),random);
    std::vector<double> in(1024*305,1.25),bias(300,.5),actual(1024*300),expected(actual.size());
    for(auto [r,c]:schedule) {
        padded_np(actual.data(),in.data(),bias.data(),r,c,c+5);
        mapped_raw(expected.data(),in.data(),bias.data(),r,c,c+5);
        if(!std::equal(actual.begin(),actual.begin()+r*c,expected.begin()))std::exit(2);
    }
    std::vector<double>a,b;
    for(size_t trial=0;trial!=11;++trial) {
        auto time=[&](mapped_function fn) {
            fn=escape(fn);auto*out=escape(actual.data());auto*input=escape(in.data());auto*offset=escape(bias.data());
            const auto begin=std::chrono::steady_clock::now();
            for(size_t rep=0;rep!=20;++rep)for(auto [r,c]:schedule) {fn(out,input,offset,r,c,c+5);asm volatile("":::"memory");}
            sink=actual[0];return std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-begin).count()/20;
        };
        if(trial%2){a.push_back(time(padded_np));b.push_back(time(mapped_raw));}
        else{b.push_back(time(mapped_raw));a.push_back(time(padded_np));}
    }
    std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
    std::printf("shuffled-padded np=%.2f raw4=%.2f ratio=%.3f\n",a[5],b[5],a[5]/b[5]);
}
int main() {fixed_mapped(false);fixed_mapped(true);mixed();}
