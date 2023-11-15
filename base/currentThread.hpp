#ifndef _CURRENTTHREAD_HPP_
#define _CURRENTTHREAD_HPP_

namespace CurrentThread
{
   extern __thread int t_cachedTid;
   void cacheTid();

   inline int GetTid()
   {
        if(__builtin_expect(t_cachedTid==0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
   }
}//namespace CurrentThread

#endif
