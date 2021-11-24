#include <iostream>
#include <algorithm>
#include <cstdio>
#define outt(x) cout<<#x<<" = "<<x<<"   "
#define hh puts("")
#define modd(x) (((x)>mod)?((x)-mod):(x))
using namespace std;
typedef long long ll;
const int INF=1000000000;
const ll mod = 1000000007;
const int maxn = 5000;
ll ans;
int dp[maxn],diff,pre[maxn],suffix[maxn];
int cnt,qwer;
char cc[maxn],ccc;
int main()
{
	#ifndef ONLINE_JUDGE
	freopen(".\\TestCase\\Number_String\\1.in","r",stdin);
	//freopen(".\\TestCase\\Number_String\\ans.txt","w",stdout);
	#endif
	while(~scanf("%s",cc+1))
	{
		cnt=1;qwer++;
		dp[1]=1;pre[1]=1;suffix[1]=1;pre[0]=pre[2]=0;suffix[0]=suffix[2]=0;
	//	printf("%s\n",cc+1);
		while(cc[cnt]!=0)
		{ 
			ccc=cc[cnt];cnt++;
	 		if(ccc=='I') diff=1;
	 		else if(ccc=='D') diff=-1;
	 		else diff=0;
	 		
	 		for(register int i=1;i<=cnt;++i)
	 		{
	 			dp[i]=0;
	 			if(diff>=0) dp[i]=modd(dp[i]+pre[i-1]);
	 			if(diff<=0) dp[i]=modd(dp[i]+suffix[i]);
	 		}
	 		pre[0]=0;suffix[cnt+1]=0;
	 		for(register int i=1;i<=cnt;++i) pre[i]=modd(pre[i-1]+dp[i]);
	 		for(register int i=cnt;i>=1;--i) suffix[i]=modd(suffix[i+1]+dp[i]);
	 		
	 		/*outt(cnt);hh;
	 		for(int i=1;i<=cnt;++i)
	 		{
	 			cout<<dp[i]<<"   ";
	 		}
	 		hh;
	 		//*/
		}		
		ans=0ll;
		for(int i=1;i<=cnt;++i) ans=modd(ans+dp[i]);
		ans%=mod;
		printf("%lld\n",ans);
	}
//	printf("qwer: %d",qwer);
  return 0;
}
/*
I?D?D
I????DID
DI
?DIIDDI
DDDDIID??
?D
??DI
?DDDD?ID
?I??
DD?D?DI?
*/

