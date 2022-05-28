package com.king.activemqtest.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.king.activemqtest.R;
import com.king.activemqtest.bean.PersonBean;

import java.util.List;

/**
 * 人员列表适配器
 */
public class PersonAdapter extends RecyclerView.Adapter<PersonAdapter.ViewHolder>{


    private List<PersonBean> list;
    private Context context;
    private OnItemLongClick onItemLongClick;

    public void setOnItemLongClick(OnItemLongClick onItemLongClick) {
        this.onItemLongClick = onItemLongClick;
    }

    public PersonAdapter(List<PersonBean> list, Context context) {
        this.list = list;
        this.context = context;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view= LayoutInflater.from(context).inflate(R.layout.adapter_person,parent,false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        holder.personTextView.setText(list.get(position).getName());
        holder.idTextView.setText(list.get(position).getRfid());
        holder.itemView.setOnLongClickListener(view -> {
            if(onItemLongClick!=null){
                onItemLongClick.onItemLongClickListener(view,holder.getLayoutPosition());
            }
            return false;
        });

    }

    @Override
    public int getItemCount() {
        return list==null?0:list.size();
    }

    class ViewHolder extends RecyclerView.ViewHolder{

        private TextView personTextView;
        private TextView idTextView;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            personTextView=itemView.findViewById(R.id.personTextView);
            idTextView=itemView.findViewById(R.id.idTextView);
        }
    }

    public interface OnItemLongClick{
        void onItemLongClickListener(View view,int position);
    }

}
