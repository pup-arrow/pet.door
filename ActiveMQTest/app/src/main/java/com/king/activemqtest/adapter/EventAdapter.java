package com.king.activemqtest.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.king.activemqtest.R;
import com.king.activemqtest.bean.StatisticsBean;

import java.util.List;

/**
 * 统计列表适配器
 */
public class EventAdapter extends RecyclerView.Adapter<EventAdapter.RfidViewHolder> {

    private List<StatisticsBean> list;
    private Context context;

    public EventAdapter(List<StatisticsBean> list, Context context) {
        this.list = list;
        this.context = context;
    }

    @NonNull
    @Override
    public RfidViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view= LayoutInflater.from(context).inflate(R.layout.adapter_event,parent,false);
        return new RfidViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull RfidViewHolder holder, int position) {
        holder.timeTextView.setText(list.get(position).getTime());
        holder.rfidTextView.setText(list.get(position).getEvent());
    }

    @Override
    public int getItemCount() {
        return list==null?0:list.size();
    }

    class RfidViewHolder extends RecyclerView.ViewHolder{

        private TextView timeTextView;
        private TextView rfidTextView;

        public RfidViewHolder(@NonNull View itemView) {
            super(itemView);
            timeTextView=itemView.findViewById(R.id.timeTextView);
            rfidTextView=itemView.findViewById(R.id.rfidTextView);
        }
    }

}
